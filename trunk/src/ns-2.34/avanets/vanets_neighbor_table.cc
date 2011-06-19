/*
 * vanets_neighbor_vehicle.cpp
 *
 *  Created on: 29/05/2011
 *      Author: roger
 */

#include "vanets_neighbor_table.h"

#include <string.h>

vanets_neighbor_table::vanets_neighbor_table(){
	data_vehicles = (neighbor_vehicle_object *)malloc(sizeof(neighbor_vehicle_object)*16);
	capacity = 16;
	size = 0;
}

bool vanets_neighbor_table::insert(neighbor_vehicle_object *obj){
	if (size == capacity)
		realloc_list();

	data_vehicles[size] = *obj;
	bcopy(obj, &data_vehicles[size], sizeof(neighbor_vehicle_object));
	size++;

	return true;
}

neighbor_vehicle_object * vanets_neighbor_table::search(neighbor_vehicle_object *obj){
	u_int32_t i = 0;

	for (i = 0; i < size; i++){
		if (compare_object(&data_vehicles[i], obj))
			return &data_vehicles[i];
	}

	return NULL;
}

/**
 * Atualiza os TTL dos nodos.
 * Caso o TTL = 0, elimina o nodo da lista.
 */
void vanets_neighbor_table::update_ttl(){
	u_int32_t i;

	for (i = 0; i < size; i++){
		--data_vehicles[i].ttl;

		if (data_vehicles[i].ttl < 0){
			data_vehicles[i] = data_vehicles[size-1];
			size--;
			i--;
		}
	}
}

