/*
 * vanets.h
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#ifndef VANETS_H_
#define VANETS_H_

#include <cmu-trace.h>
#include <priqueue.h>
#include <classifier/classifier-port.h>

// ======================================================================
//  WFRP Routing Agent : the routing protocol
// ======================================================================

class XFXVanets: public Agent {

public:
	XFXVanets(nsaddr_t id);

	//virtual void recv(Packet *p, Handler *);

	int command(int, const char * const *);

	// Agent Attributes
	nsaddr_t index; // node address (identifier)
	nsaddr_t seqno; // beacon sequence number (used only when agent is sink)

	// Node Location
	uint32_t posx; // position x;
	uint32_t posy; // position y;


	// Routing Table Management
	//virtual void rt_insert(nsaddr_t src, u_int32_t id, nsaddr_t nexthop,
	//		u_int32_t xpos, u_int32_t ypos, u_int8_t hopcount);
	//virtual void rt_remove(RouteCache *rt);
	//virtual void rt_purge();
	//virtual RouteCache* rt_lookup(nsaddr_t dst);

	// Timers
	//virtual wfrpBeaconTimer bcnTimer;
	//virtual wfrpRouteCacheTimer rtcTimer;

	// Caching Head
	//virtual wfrp_rtcache rthead;

	// Send Routines
	/*virtual void send_beacon();
	virtual void send_error(nsaddr_t unreachable_destination);
	virtual void forward(Packet *p, nsaddr_t nexthop, double delay);*/

	// Recv Routines
	/*virtual void recv_data(Packet *p);
	virtual void recv_wfrp(Packet *p);
	virtual void recv_beacon(Packet *p);
	virtual void recv_error(Packet *p);*/

	// Position Management
	//virtual void update_position();

	//  A mechanism for logging the contents of the routing table.
	Trace *logtarget;

	// A pointer to the network interface queue that sits between the "classifier" and the "link layer"
	PriQueue *ifqueue;

	// Port classifier for passing packets up to agents
	PortClassifier *dmux_;
};

#endif /* VANETS_H_ */
