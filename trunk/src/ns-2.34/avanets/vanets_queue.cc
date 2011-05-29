/*
 * vanets_queue.cpp
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#include "vanets_queue.h"

using namespace std;

/**
 * Construtor
 */
vanets_queue::vanets_queue(){
	size = 0;
	first = NULL;
}

bool vanets_queue::enqueue(object_queue *object){
	object_queue *nobj = new object_queue();
	string str;
	stringstream out;

	nobj->x = object->x;
	nobj->y = object->y;
	nobj->next = NULL;
	nobj->previous = nobj;

	if (size == 0){
		first = nobj;
		last = nobj;
		size++;

		out << "Elemento (x, y) = (" << object->x << ", " << object->y << ") inserido (primeiro). Size = " << size;
		str = out.str();
	} else {
		nobj->previous = last;
		nobj->next = NULL;

		last->next = nobj;
		last = nobj;
		size++;

		out << "Elemento (x, y) = (" << object->x << ", " << object->y << ") inserido (primeiro). Size = " << size;
		str = out.str();
	}

	generate_log(str);

	return true;
}

object_queue * vanets_queue::search(u_int32_t x, u_int32_t y){
	object_queue *next = NULL;
	stringstream out;

	next = first;

	while (next != NULL){
		if (next->x == x && next->y == y){
			out << "Elemento (x, y) = (" << x << ", " << y << ") localizado na queue";
			generate_log(out.str());

			return next;
		}

		next = next->next;
	}

	out << "Elemento (x, y) = (" << x << ", " << y << ") não localizado na queue";
	generate_log(out.str());

	return NULL;
}

object_queue *vanets_queue::dequeue(){
	object_queue *ret = first;
	stringstream out;

	if (size == 0){
		generate_log("Não é possível retirar elemento da queue. Size queue== 0");
		return NULL;
	}

	if (size > 1){
		first->next->previous = first->next;
		first = first->next;
	} else {
		first = NULL;
		last = NULL;
	}

	size--;
	out << "Elemento (x, y) = (" << ret->x << ", " << ret->y << ") eliminado da queue. Size = " << size;
	generate_log(out.str());

	return ret;
}

string vanets_queue::to_string(){
	stringstream cout;
	object_queue *next = first;

	while (next != NULL){
		cout << "(" << next->x << ", " << next->y << ")";
		next = next->next;
	}

	cout << endl;

	return cout.str();
}
