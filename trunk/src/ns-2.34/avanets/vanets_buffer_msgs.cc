/*
 * vanets_buffer_msgs.cc
 *
 * Classe que mantem as mensagens recebidas.
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#include "vanets_buffer_msgs.h"

#include <iostream>
#include <string>
// para o método bcopy
#include <strings.h>

using namespace std;

vanets_buffer_msgs::vanets_buffer_msgs(){
	default_init_size = 256;
	capacity = default_init_size;
	size = 0;

	data_msgs = new packet_general_message[size];
}

packet_general_message * vanets_buffer_msgs::insert(packet_general_message *msg){
	stringstream out;

	if (size == capacity || size == 0 || capacity == 0)
		realloc_list();

	if (size != capacity) {
		bcopy(msg, &data_msgs[size++], sizeof(packet_general_message));
		out << "Elemento (" << data_msgs[size-1].to_string() << ") inserido na lista. Size = " << size;
		generate_log(out.str());
	} else
		return NULL;

	return &data_msgs[size-1];
}

packet_general_message * vanets_buffer_msgs::search(packet_general_message *msg){
	for (u_int32_t i = 0; i < size; i++){
		if (compare_object(msg, &data_msgs[i]))
			return &data_msgs[i];
	}

	return NULL;
}

void vanets_buffer_msgs::delete_object(u_int32_t pos){
	stringstream out;

	if (pos < 0 || pos >= size)
		return;

	out << "Elemento (" << data_msgs[pos].to_string() << ") deletado da lista";
	generate_log(out.str());

	bcopy(&data_msgs[size-1], &data_msgs[pos], sizeof(packet_general_message));
	size--;
}

packet_general_message * vanets_buffer_msgs::get_object(u_int32_t pos){
	if (pos >= size)
		return NULL;

	return &data_msgs[pos];
}
