
#ifndef LIST_MOBILE_NODES_H_
#define LIST_MOBILE_NODES_H_

#include <iostream>
#include <vector>
using namespace std;

class list_mobile_nodes;

class list_mobile_nodes {
public:
	static list_mobile_nodes *instance();
	void add(void *s);
	size_t size();
	vector<void *>get_pointer();
private:
	vector<void *>pointers;
	list_mobile_nodes(){};
	static list_mobile_nodes *s_instance;
};


#endif /* LIST_MOBILE_NODES_H_ */
