#include <random.h>
#include <cmu-trace.h>
#include <energy-model.h>
#include <common/mobilenode.h>

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
			btimerXfx.handle((Event*) 0);
			htimerXfx.handle((Event*) 0);
			return TCL_OK;
		}

		// Start Beacon Timer (which sends beacon message)
		/*if (strncasecmp(argv[1], "sink", 4) == 0) {
			bcnTimer.handle((Event*) 0);

			printf("N (%.6f): sink node is set to %d, start beaconing  \n",
					CURRENT_TIME, index);

			return TCL_OK;
		}*/
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
 * A cada segundo, manda uma mensagem de hello para todos os vizinhos
 */
void HelloTimerXFX::handle(Event*) {
	agent->sendHello();
	Scheduler::instance().schedule(this, &intr, TIME_HELLO_MESSAGE);

	fprintf(stdout, "I'm %d\n", agent->index);
	fprintf(stdout, "My route: ");


	cout << list_mobile_nodes::instance()->size() << endl;
}

/* ========================================================================= */
/**
 * Exclui vizinhos com TTL = 0
 */
void XFXVanets::id_purge(){
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

	rh->rp_type = XFX_MSG_HELLO;
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
void XFXVanets::recv(Packet *p, Handler *){
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);

	if (ch->ptype() == PT_XFXVanets) {
		ih->ttl_ -= 1;
		recvXFX(p);
		return;
	}
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
		case XFX_MSG_HELLO:
			recvHelloMsg(p);
			break;

		default:
			fprintf(stderr, "Invalid XFX type (%x)\n", ah->ah_type);
			exit(1);
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
	if (obj != NULL) // atualizar aqui o caminho do cara
		obj->ttl = 3;
	else
		neighbor_vehicles->insert(look);

	delete look;
	Packet::free(p);
}

