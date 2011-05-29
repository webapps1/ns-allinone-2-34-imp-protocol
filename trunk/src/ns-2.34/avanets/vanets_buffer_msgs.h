/*
 * vanets_buffer_msgs.h
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#ifndef VANETS_BUFFER_MSGS_H_
#define VANETS_BUFFER_MSGS_H_

#include <iostream>
#include <cstdlib>
#include "vanets_packet.h"

using namespace std;

class vanets_buffer_msgs {
private:
	u_int32_t size, capacity;
	u_int32_t default_init_size;
	packet_general_message *data_msgs;

	void realloc_list(){
		capacity = (capacity > 0)? capacity : 1;
		data_msgs = (packet_general_message *)realloc(data_msgs, sizeof(packet_general_message)*capacity*2);

		if (data_msgs != NULL){
			capacity *= 2;
		} else {
			size = 0;
			capacity = 0;
		}
	}

	bool compare_object(packet_general_message *msg1, packet_general_message *msg2){
		if (msg1->id_msg == msg2->id_msg && msg1->destination == msg2->destination)
			return true;

		return false;
	}

	void generate_log(string srt){
		#ifdef DEBUG
		cout << srt << endl;
		#endif
	}
public:
	vanets_buffer_msgs();
	packet_general_message *insert(packet_general_message *);
	void delete_object(u_int32_t);
	packet_general_message * get_object(u_int32_t);
	packet_general_message * search(packet_general_message *);

	string to_string(){
		stringstream out;

		for (u_int32_t i = 0; i < size; i++){
			out << data_msgs[i].to_string() << endl;
		}

		return out.str();
	}
};

#endif /* VANETS_BUFFER_MSGS_H_ */
