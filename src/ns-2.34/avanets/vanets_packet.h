/*
 * vanets.h
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#ifndef VANETS_PACKET
#define VANETS_PACKET

#include <iostream>
#include <config.h>
#include <packet.h>
#include "vanets_queue.h"

/**
 * Macros XFX Routing
 */
#define HDR_XFX(p)		((struct hdr_xfx*)hdr_xfx::access(p))
#define HDR_XFX_REPLY(p)	((struct hdr_xfx_reply*)hdr_xfx::access(p))

/**
 * General XFX Header
 */
struct hdr_xfx {
	u_int8_t ah_type;
	static int offset_; // required by PacketHeaderManager
	inline static int& offset() {
		return offset_;
	}
	inline static hdr_xfx* access(const Packet* p) {
		return (hdr_xfx*) p->access(offset_);
	}
};

/**
 * Utilizado no hello message
 */
struct hdr_xfx_reply {
	u_int8_t rp_type; // tipo do pacote
	u_int8_t reserved[2];
	nsaddr_t rp_dst; // ip de destino
	u_int32_t rp_dst_seqno; // Destination Sequence Number
	nsaddr_t rp_src; // ip de origem
	double rp_lifetime; // Lifetime
	double rp_timestamp; // when corresponding REQ sent;

	inline int size() {
		int sz = 0;
		sz = 7 * sizeof(u_int32_t);
		return sz;
	}
};

/**
 * Estrutura com os pacotes utilizados no hello mensagem
 *
 * até agora, nao foi usado
 */
typedef struct{
	nsaddr_t id_vehicle; /* id do carro*/
	vanets_queue path; /* conjunto de arestas  (x, y) */

	inline static int size(){
		return 0;
	}
} packet_hello_message;

/**
 * Estrutura com uma determinada mensagem
 */
typedef struct {
	nsaddr_t id_msg; /** id da mensagem */
	int64_t destination; /* destino da mensagem */
	u_int16_t ttl /* time to live, inicializado com 30 */;

	string to_string(){
		stringstream out;
		out << "Id: " << id_msg << "; Destination: " << destination << "; Ttl: " << ttl;
		return out.str();
	}

	inline static int size(){
		return 0;
	}
} packet_general_message;

/**
 * Estrutura com para a tabela "neighbor vehicle"
 */
typedef struct {
	nsaddr_t id_vehicle;
	vanets_queue path;
	u_int16_t ttl;

	string to_string(){
		stringstream out;
		out << "Id: " << id_vehicle << "; Ttl: " << ttl;
		return out.str();
	}
} neighbor_vehicle_object;

#endif /* VANETS_H_ */
