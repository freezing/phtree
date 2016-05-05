/*
 * NodeAddressContent.h
 *
 *  Created on: Feb 29, 2016
 *      Author: max
 */

#ifndef NODEADDRESSCONTENT_H_
#define NODEADDRESSCONTENT_H_

#include "util/MultiDimBitset.h"

template <unsigned int DIM>
class Node;

template <unsigned int DIM>
struct NodeAddressContent {
	int id;
	bool exists;
	bool hasSubnode;
	unsigned long address;
	union {
		Node<DIM>* subnode;
		unsigned long* suffixStartBlock;
	};
};


#endif /* NODEADDRESSCONTENT_H_ */
