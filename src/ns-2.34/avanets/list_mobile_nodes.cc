#include "list_mobile_nodes.h"

list_mobile_nodes * list_mobile_nodes::s_instance = NULL;

list_mobile_nodes *list_mobile_nodes::instance(){
	if (!s_instance)
		s_instance = new list_mobile_nodes();

	return s_instance;
}

void list_mobile_nodes::add(void *a){
	pointers.push_back(a);
}

size_t list_mobile_nodes::size(){
	return pointers.size();
}

vector<void *> list_mobile_nodes::get_pointer(){
	return pointers;
}
