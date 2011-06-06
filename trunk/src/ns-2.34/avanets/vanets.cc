/*
 * vanets.cpp
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#include "vanets.h"
#include "vanets_packet.h"

#include <random.h>
#include <cmu-trace.h>
#include <energy-model.h>

#define max(a,b)	((a) > (b)? (a) : (b))
//int hdr_wfrp::offset_;

/* ========================================================================= */
static class XFXVanetsHeaderClass: public PacketHeaderClass {
public:
	XFXVanetsHeaderClass() :
		PacketHeaderClass(
						"PacketHeader/XFXVanets",
						max(sizeof(packet_hello_message), sizeof(packet_general_message))) {
		//bind_offset(&hdr_wfrp::offset_);
		printf("Inicializa classe XFXVanetsHeaderClass - vanets.cc\n");
	}
} class_rtProtoXFXVanets_hdr;

/* ========================================================================= */
static class XFXVanetsclass: public TclClass {
public:
	XFXVanetsclass() :
		TclClass("Agent/XFXVanets") {
	}

	TclObject* create(int argc, const char* const * argv) {
		fprintf(stdout, "Método create XFXVanetsclass\n");
		assert(argc == 5);
		return (new XFXVanets((nsaddr_t) Address::instance().str2addr(argv[4])));

	}
} class_rtProtoXFXVanets;

/* ========================================================================= */
XFXVanets::XFXVanets(nsaddr_t id) : Agent(PT_XFXVanets) {

	printf("XFXVanets: Routing agent is initialized for node %d \n", id	);

	index = id;
	seqno = 1;

	//LIST_INIT(&rthead);
	posx = 0;
	posy = 0;

	logtarget = 0;
	ifqueue = 0;
}

/* ========================================================================= */
int XFXVanets::command(int argc, const char* const * argv) {
	if (argc == 2) {
		Tcl& tcl = Tcl::instance();

		if (strncasecmp(argv[1], "id", 2) == 0) {
			tcl.resultf("%d", index);
			return TCL_OK;
		}

		if (strncasecmp(argv[1], "start", 5) == 0) {
			//rtcTimer.handle((Event*) 0);
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

