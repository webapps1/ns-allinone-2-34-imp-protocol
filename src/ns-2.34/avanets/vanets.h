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
#include <avanets/vanets_neighbor_table.h>
#include <packet.h>

class XFXVanets;

class BroadcastTimerXFX: public Handler {
public:
	BroadcastTimerXFX(XFXVanets *a) : agent(a) { }
	void handle(Event*);
private:
	XFXVanets *agent;
	Event intr;
};

class HelloTimerXFX: public Handler {
public:
	HelloTimerXFX(XFXVanets *a) : agent(a) {	}
	void handle(Event*);
private:
	XFXVanets *agent;
	Event intr;
};

// ======================================================================
// Types of Messages
// ======================================================================
#define XFX_MSG_HELLO 0x1

// ======================================================================
//  Timers (Broadcast ID, Hello)
// ======================================================================
#define TIME_HELLO_MESSAGE 1
#define TIME_BROADCAST_MESSAGE 1

// ======================================================================
//  XFXVanets Routing Agent : the routing protocol
// ======================================================================

class XFXVanets: public Agent {
    friend class BroadcastTimerXFX;
    friend class HelloTimerXFX;
public:
	XFXVanets(nsaddr_t id);
	int command(int, const char * const *);
	void id_purge();
	void sendHello();
	void recv(Packet *, Handler*);
	void recvXFX(Packet *);
	void recvHelloMsg(Packet *);

	nsaddr_t index; // node address (identifier)
	nsaddr_t seqno; // beacon sequence number (used only when agent is sink)

	uint32_t posx; // position x;
	uint32_t posy; // position y;

	Trace *logtarget;
	PriQueue *ifqueue;
	PortClassifier *dmux_;

protected:
	BroadcastTimerXFX  btimerXfx;
	HelloTimerXFX htimerXfx;
	vanets_neighbor_table *neighbor_vehicles;
};

#endif /* VANETS_H_ */
