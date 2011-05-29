/*
 * vanets.h
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#ifndef VANETS_PACKET
#define VANETS_PACKET

#include <iostream>

#include "vanets_queue.h"

/**
 * Estrutura com os pacotes utilizados no hello mensagem
 */
typedef struct packet_hello_message {
	u_int64_t id_vehicle; /* id do carro*/
	vanets_queue path; /* conjunto de arestas  (x, y) */
} packet_hello_message;

/**
 * Estrutura com uma determinada mensagem
 */
typedef struct {
	u_int64_t id_msg; /** id da mensagem */
	u_int64_t destination; /* destino da mensagem */
	u_int16_t ttl /* time to live, inicializado com 30 */;

	string to_string(){
		stringstream out;
		out << "Id: " << id_msg << "; Destination: " << destination << "; Ttl: " << ttl;
		return out.str();
	}
} packet_general_message;

/**
 * Estrutura com para a tabela "neighbor vehicle"
 */
typedef struct {
	u_int64_t id_vehicle;
	vanets_queue path;
	u_int16_t ttl;

	string to_string(){
		stringstream out;
		out << "Id: " << id_vehicle << "; Ttl: " << ttl;
		return out.str();
	}
} neighbor_vehicle_table;

#endif /* VANETS_H_ */
