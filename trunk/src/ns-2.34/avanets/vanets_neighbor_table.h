/*
 * vanets_neighbor_vehicle.h
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#ifndef VANETS_NEIGHBOR_TABLE_H_
#define VANETS_NEIGHBOR_TABLE_H_

#include <iostream>
#include <cstdlib>
#include "vanets_packet.h"

class vanets_neighbor_table {
private:
	u_int32_t size, capacity;
	neighbor_vehicle_object *data_vehicles;

	void realloc_list(){
		capacity = (capacity > 0)? capacity : 1;
		data_vehicles = (neighbor_vehicle_object *)realloc(data_vehicles, sizeof(neighbor_vehicle_object)*capacity*2);

		if (data_vehicles != NULL){
			capacity *= 2;
		} else {
			size = 0;
			capacity = 0;
		}
	}

	bool compare_object(neighbor_vehicle_object *msg1, neighbor_vehicle_object *msg2){
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
	vanets_neighbor_table();
	bool insert(neighbor_vehicle_object *obj);
	neighbor_vehicle_object * search (neighbor_vehicle_object *);
	void update_ttl();
};

#endif /* VANETS_NEIGHBOR_VEHICLE_H_ */
