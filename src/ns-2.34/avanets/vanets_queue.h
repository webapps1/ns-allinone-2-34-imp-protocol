/*
 * vanets_queue.h
 *
 * Classe utilizada para manter uma queue com os caminhos que um determinado carro irá fazer.
 * Caminho este, representado por um conjunto de arestas (x, y) (x e y = u_int32_t)
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#ifndef VANETS_QUEUE_H_
#define VANETS_QUEUE_H_

#include <iostream>
#include <sys/types.h>
#include <string>
#include <sstream>

using namespace std;

typedef struct object_queue {
	u_int32_t x;
	u_int32_t y;
	object_queue *next;
	object_queue *previous;
} object_queue;


class vanets_queue {
	private:
		u_int32_t size;
		object_queue * first;
		object_queue * last;
	public:
		vanets_queue();
		bool enqueue(object_queue *object);
		object_queue * search(u_int32_t x, u_int32_t y);
		object_queue * dequeue();
		string to_string();
		void generate_log(string srt) {
			#ifdef DEBUG
				cout << srt << endl;
			#endif
		}
};

#endif /* VANETS_QUEUE_H_ */
