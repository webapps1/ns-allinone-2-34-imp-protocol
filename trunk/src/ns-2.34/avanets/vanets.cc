#include <random.h>
#include <cmu-trace.h>
#include <energy-model.h>
#include <common/mobilenode.h>
#include <time.h>
#include "vanets.h"
#include "vanets_packet.h"

#include "list_mobile_nodes.h"

#define max(a,b)	((a) > (b)? (a) : (b))
#define CURRENT_TIME    Scheduler::instance().clock()
int hdr_xfx::offset_;

/* ========================================================================= */
static class XFXVanetsHeaderClass: public PacketHeaderClass {
public:
	XFXVanetsHeaderClass() :
				PacketHeaderClass(
						"PacketHeader/XFXVanets",
						max(sizeof(packet_hello_message), sizeof(packet_general_message))) {
		bind_offset(&hdr_xfx::offset_);
	}
} class_rtProtoXFXVanets_hdr;

/* ========================================================================= */
static class XFXVanetsclass: public TclClass {
public:
	XFXVanetsclass() :
		TclClass("Agent/XFXVanets") {
	}

	TclObject* create(int argc, const char* const * argv) {
		assert(argc == 5);
		return (new XFXVanets((nsaddr_t) Address::instance().str2addr(argv[4])));

	}
} class_rtProtoXFXVanets;

/* ========================================================================= */
int XFXVanets::command(int argc, const char* const * argv) {
	if (argc == 2) {
		Tcl& tcl = Tcl::instance();

		if (strncasecmp(argv[1], "id", 2) == 0) {
			tcl.resultf("%d", index);
			return TCL_OK;
		}

		if (strncasecmp(argv[1], "start", 5) == 0) {
			/** chama btimerXfx.handle se for móvel */
			if (((MobileNode *)list_mobile_nodes::instance()->get_pointer()[index])->kind == 2)
				btimerXfx.handle((Event*) 0);

			htimerXfx.handle((Event*) 0);
			return TCL_OK;
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "index") == 0) {
			index = atoi(argv[2]);
			return TCL_OK;
		} else if (strcmp(argv[1], "log-target") == 0 || strcmp(argv[1],
				"tracetarget") == 0) {
			logtarget = (Trace*) TclObject::lookup(argv[2]);
			if (logtarget == 0)
				return TCL_ERROR;
			return TCL_OK;
		} else if (strcmp(argv[1], "drop-target") == 0) {
			return TCL_OK;
		} else if (strcmp(argv[1], "if-queue") == 0) {
			ifqueue = (PriQueue*) TclObject::lookup(argv[2]);

			if (ifqueue == 0)
				return TCL_ERROR;
			return TCL_OK;
		} else if (strcmp(argv[1], "port-dmux") == 0) {
			dmux_ = (PortClassifier *) TclObject::lookup(argv[2]);
			if (dmux_ == 0) {
				fprintf(stderr, "%s: %s lookup of %s failed\n", __FILE__,
						argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		}
	}

	return Agent::command(argc, argv);
}

/* ========================================================================= */
XFXVanets::XFXVanets(nsaddr_t id) :
	Agent(PT_XFXVanets), btimerXfx(this), htimerXfx(this) {
	printf("XFXVanets: Routing agent is initialized for node %d \n", id);

	index = id;
	seqno = 1;

	neighbor_vehicles = new vanets_neighbor_table();
	posx = 0;
	posy = 0;

	logtarget = 0;
	ifqueue = 0;
}

/* ========================================================================= */
/* ================================= TIMERS ================================ */
/* ========================================================================= */
/**
 * A cada segundo, verifica se a lista possui vizinhos com TTL = 0
 */
void BroadcastTimerXFX::handle(Event*){
	agent->id_purge();
	Scheduler::instance().schedule(this, &intr, TIME_BROADCAST_MESSAGE);
}

/* ========================================================================= */
/**
 * A cada segundo, manda uma mensagem de hello para todos os vizinhos.
 */
void HelloTimerXFX::handle(Event*) {
	agent->sendHello();
	Scheduler::instance().schedule(this, &intr, TIME_HELLO_MESSAGE);
}

/* ========================================================================= */
/**
 * Exclui vizinhos com TTL = 0.
 * Só é executado por nodos móveis (carros).
 */
void XFXVanets::id_purge() {
	neighbor_vehicles->update_ttl();
}

/* ========================================================================= */
/* ============================= AGENT'S METHOD ============================ */
/* ========================================================================= */
/**
 * Faz um broadcast com a mensagem de HELLO.
 */
void XFXVanets::sendHello() {
	Packet *p = Packet::alloc();
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_xfx_reply *rh = HDR_XFX_REPLY(p);

	if (((MobileNode *)list_mobile_nodes::instance()->get_pointer()[index])->kind == 2)
		rh->rp_type = XFX_MSG_HELLO_MOVEL;
	else
		rh->rp_type = XFX_MSG_HELLO_STATIC;

	rh->rp_dst = index;
	rh->rp_dst_seqno = seqno;
	rh->rp_lifetime = 4;

	// ch->uid() = 0;
	ch->ptype() = PT_XFXVanets;
	ch->size() = IP_HDR_LEN + rh->size();
	ch->iface() = -2;
	ch->error() = 0;
	ch->addr_type() = NS_AF_NONE;
	ch->prev_hop_ = index;

	ih->saddr() = index;
	ih->daddr() = IP_BROADCAST;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	ih->ttl_ = 1;

	Scheduler::instance().schedule(target_, p, 0.0);
}

/* ========================================================================= */
/**
 * Recebe as mensagens
 */
void XFXVanets::recv(Packet *p1, Handler *){
	struct hdr_cmn *ch1 = HDR_CMN(p1);
	struct hdr_ip *ih1 = HDR_IP(p1);

	if (ch1->ptype() == PT_XFXVanets) {
		ih1->ttl_--;
		recvXFX(p1);
		return;
	}

	cout << "Id: " << index << endl;
	cout << ih1->saddr() << endl;
	cout << ih1->daddr() << endl;

	//  Must be a packet I'm originating
	if((ih1->saddr() == index) && (ch1->num_forwards() == 0)) {
		if (ch1->ptype() != PT_TCP && ch1->ptype() != PT_ACK) {
			ch1->size() += IP_HDR_LEN;
		}
	} else if(ih1->saddr() == index) {// I received a packet that I sent.  Probably routing loop.
   		drop(p1, DROP_RTR_ROUTE_LOOP);
		return;
	} else { //  Packet I'm forwarding...
		if(--ih1->ttl_ == 0) {
			drop(p1, DROP_RTR_TTL);
			return;
   		}
	}

	/** aqui, validar o caminho para um determinada mensagem */

	bufferMsgs.push_back(p1);
}

/* ========================================================================= */
/**
 * Verifica o que deve ser feito com o pacote recebido.
 */
void XFXVanets::recvXFX(Packet *p) {
	struct hdr_xfx *ah = HDR_XFX(p);

	assert(HDR_IP(p)->sport() == RT_PORT);
	assert(HDR_IP(p)->dport() == RT_PORT);

	/**
	 * Dá um destino a mensagem
	 */
	switch (ah->ah_type) {
		case XFX_MSG_HELLO_MOVEL:
			if (((MobileNode *)list_mobile_nodes::instance()->get_pointer()[index])->kind == 2)
				recvHelloMsg(p);
			break;

		case XFX_MSG_HELLO_STATIC:
			sendMsgStNodo(p);
			break;

		case XFX_MSG_NORMAL:
			cout << "Sou "<< index << " e recebi uma normal message" << endl;
			break;

		case XFX_MSG_FORWARD:
			handleForward(p);
			break;

		default:
			fprintf(stderr, "Invalid XFX type (%x)\n", ah->ah_type);
	}
}

/* ========================================================================= */
/**
 * Ao receber um pacote HELLO.
 * Verifica se o nodo de origem está na heighbor table.
 * Caso não, insere na table. Caso sim, atualiza o ttl.
 */
void XFXVanets::recvHelloMsg(Packet *p){
	struct hdr_xfx_reply *rp = HDR_XFX_REPLY(p);
	neighbor_vehicle_object *obj, *look;

	look = new neighbor_vehicle_object;
	look->id_vehicle = rp->rp_dst;
	look->ttl = 3;

	obj = neighbor_vehicles->search(look);
	if (obj != NULL)// atualizar aqui o caminho do cara
		obj->ttl = 3;
	else
		neighbor_vehicles->insert(look);

	/**
	 * Percorre o buffer de mensagens e verifica quem possui o menor
	 * caminho para entregar cada mensagem.
	 */
	sendMsgMinorDistance(rp->rp_dst);

	delete look;
	Packet::free(p);
}

/* ========================================================================= */
/**
 * O método abaixo é sempre utilizado quando um nodo recebe um hello de um nodo estático.
 * Percorre o buffer e verifica se existe alguma mensagem que possa se enviada para o nodo estático.
 */
void XFXVanets::sendMsgStNodo(Packet *p){
	list<void *>::iterator it;
	struct hdr_cmn *ch;
	struct hdr_ip *ih = HDR_IP(p), *ih2;
	struct hdr_xfx_reply *rh;
	Packet *temp;

	list<list<void *>::iterator>toDelete;

	cout << "Static " << endl;
	cout << "Id: " << index << endl;
	cout << ih->saddr() << endl;
	cout << ih->daddr() << endl;
	cout << "Size: " << bufferMsgs.size() << endl;

	for (it = bufferMsgs.begin(); it != bufferMsgs.end(); it++){
		temp = (Packet *)*it;
		ch = HDR_CMN(temp);
		ih = HDR_IP(temp);
		rh = HDR_XFX_REPLY(temp);

		ih2 = HDR_IP(p);

		/**
		 * Verifica, se o destino do pacote do buffer, é a origem do pacote recebido
		 * do novo estático.
		 */
		if (ih->daddr() == ih2->saddr()){
			/**
			 * Forward packet
			 */
			rh->rp_type = XFX_MSG_NORMAL;

			// ch->uid() = 0;
			ch->ptype() = PT_XFXVanets;
			ch->iface() = -2;
			ch->error() = 0;
			ch->addr_type() = NS_AF_NONE;

			ih->sport() = RT_PORT;
			ih->dport() = RT_PORT;

			if (ih->ttl_ >= 0){
				Scheduler::instance().schedule(target_, temp, 0.0);
				cout << "Mensagem enviada: " << ih->saddr() << " ; " << ih->daddr() << " " << endl;
			} else
				drop(p, DROP_RTR_TTL);

			/**
			 * This list doesn't work correct.
			 * When we have one object in this list .. after an erase ... a segmentation fault is showed.
			 */
			toDelete.push_back(it);
		}
	}

	list<list<void *>::iterator>::iterator itDel;

	for (itDel = toDelete.begin(); itDel != toDelete.end(); itDel++){
		bufferMsgs.erase(*itDel);
	}

}

/* ========================================================================= */
/**
 * Percorre o buffer de mensagens e verifica as distâncias.
 * O nodo com o menor caminho, leva a mensagem até o destino.
 */
void XFXVanets::sendMsgMinorDistance(nsaddr_t ip) {
	list<void *>::iterator it;
	MobileNode *store, *ipM, *nM;
	struct hdr_ip *ih;
	list<list<void *>::iterator>toDelete;
	/**
	 * Percorre a lista com as mensagens armazenadas
	 */
	store = (MobileNode *)list_mobile_nodes::instance()->get_pointer()[index];
	ipM = (MobileNode *)list_mobile_nodes::instance()->get_pointer()[ip];

	for (it = bufferMsgs.begin(); it != bufferMsgs.end(); it++){
		ih = HDR_IP((Packet *)*it);
		nM = (MobileNode *)list_mobile_nodes::instance()->get_pointer()[ih->daddr()];
		/**
		 * Calcula a distância entre o nodo store e o nodo da mensagem
		 */
		int dist_store_msg = store->check_distance(nM->X_, nM->Y_, nM->Z_);
		int dist_ipn_msg = ipM->check_distance(nM->X_, nM->Y_, nM->Z_);

		cout << dist_store_msg << " " << dist_ipn_msg << endl;

		/**
		 * Verifica quem possui a menor distância e faz o forward conforme necessário.
		 */
		if (dist_ipn_msg != -1){
			/**
			 * Repassa a mensagem para o ip com o menor caminho
			 */
			if (dist_store_msg == -1 || dist_store_msg > dist_ipn_msg){
				forward(ip, (Packet *)*it);
				toDelete.push_back(it);
			}
		}
	}

	list<list<void *>::iterator>::iterator itDel;
	for (itDel = toDelete.begin(); itDel != toDelete.end(); itDel++){
		bufferMsgs.erase(*itDel);
	}
}

/**
 * Forward message
 */
void XFXVanets::forward(nsaddr_t ip, Packet *packet)
{
	Packet *p = Packet::alloc();
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_xfx_forward *rh = HDR_XFX_FORWARD(p);

	rh->rp_type = XFX_MSG_FORWARD;
	rh->rp_dst = index;
	rh->rp_dst_seqno = seqno;
	rh->rp_lifetime = 4;
	rh->data = *packet;

	// ch->uid() = 0;
	ch->ptype() = PT_XFXVanets;
	ch->size() = IP_HDR_LEN + rh->size();
	ch->iface() = -2;
	ch->error() = 0;
	ch->addr_type() = NS_AF_NONE;
	ch->prev_hop_ = index;

	ih->saddr() = index;
	ih->daddr() = ip;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	ih->ttl_ = 30;

	Scheduler::instance().schedule(target_, p, 0.0);
}

/**
 * Trata o forward recebido
 */
void XFXVanets::handleForward(Packet *p) {
	struct hdr_cmn *ch = HDR_CMN(&HDR_XFX_FORWARD(p)->data);
	struct hdr_ip *ih = HDR_IP(&HDR_XFX_FORWARD(p)->data);
	struct hdr_xfx_reply *rh = HDR_XFX_REPLY(&HDR_XFX_FORWARD(p)->data);

	cout << "Recebi forward Id: " << index << endl;
	cout << "Dados do forward" << endl;
	cout << ih->saddr() << endl;
	cout << ih->daddr() << endl;

	bufferMsgs.push_back(&HDR_XFX_FORWARD(p)->data);
}
