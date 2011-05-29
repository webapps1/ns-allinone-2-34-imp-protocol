/*
 * main.cc
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#include <iostream>

#include "vanets_packet.h"
#include "vanets_queue.h"
#include "vanets_buffer_msgs.h"

using namespace std;

int main(int argc, char **argv)
{
	vanets_queue *queue = new vanets_queue();
	object_queue object;

	for (int i =0; i < 15; i++){
		object.x = i; object.y = i+1;
		queue->enqueue(&object);
	}

	queue->search(0, 1);
	queue->dequeue();
	queue->dequeue();
	queue->search(0, 1);
	queue->search(10, 11);
	queue->search(10, 15);


	cout << queue->to_string();

	delete queue;

	vanets_buffer_msgs * buffer = new vanets_buffer_msgs();
	packet_general_message msg;

	for (int i =0; i < 15; i++){
		msg.id_msg = i+1;
		msg.destination = i+2;
		msg.ttl = 30;

		buffer->insert(&msg);
	}

	buffer->delete_object(14);
	buffer->delete_object(7);
	buffer->delete_object(2);

	cout << buffer->to_string();

	delete buffer;
}
