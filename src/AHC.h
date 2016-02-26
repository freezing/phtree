/*
 * AHC.h
 *
 *  Created on: Feb 25, 2016
 *      Author: max
 */

#ifndef SRC_AHC_H_
#define SRC_AHC_H_

#include "Node.h"
#include <vector>

using namespace std;

class AHC: public Node {
public:
	AHC(size_t dim, size_t valueLength);
	virtual ~AHC();

protected:
	vector<bool> filled_;
	vector<bool> hasSubnode_;
	vector<Node *> subnodes_;
	// entry -> value -> bit
	vector<vector<vector<bool>>> suffixes_;

	Node::NodeAddressContent lookup(long address);
	void insertAtAddress(long hcAddress, vector<vector<bool>>* suffix);
	void insertAtAddress(long hcAddress, Node* subnode);
	ostream& output(ostream& os, size_t depth);
};

#endif /* SRC_AHC_H_ */
