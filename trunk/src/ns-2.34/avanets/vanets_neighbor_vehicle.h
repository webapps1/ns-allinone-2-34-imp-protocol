/*
 * vanets_neighbor_vehicle.h
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#ifndef VANETS_NEIGHBOR_VEHICLE_H_
#define VANETS_NEIGHBOR_VEHICLE_H_

#include <iostream>
#include <cstdlib>
#include "vanets_packet.h"

class vanets_neighbor_vehicle {
private:
	u_int32_t size, capacity;
	u_int32_t default_init_size;
	neighbor_vehicle_table *data_msgs;

	void realloc_list(){
		capacity = (capacity > 0)? capacity : 1;
		data_msgs = (neighbor_vehicle_table *)realloc(data_msgs, sizeof(neighbor_vehicle_table)*capacity*2);

		if (data_msgs != NULL){
			capacity *= 2;
		} else {
			size = 0;
			capacity = 0;
		}
	}

	bool compare_object(neighbor_vehicle_table *msg1, neighbor_vehicle_table *msg2){
		if (msg1->id_vehicle == msg2->id_vehicle)
			return true;

		return false;
	}

	void generate_log(string srt){
		#ifdef DEBUG
		cout << srt << endl;
		#endif
	}
public:
	vanets_neighbor_vehicle();
	bool insert(neighbor_vehicle_table *obj);
	neighbor_vehicle_table * search (neighbor_vehicle_table *);
	void update_ttl();
	void clear_tll_zero();
};

#endif /* VANETS_NEIGHBOR_VEHICLE_H_ */
