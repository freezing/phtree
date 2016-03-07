/*
 * PHTree.h
 *
 *  Created on: Feb 25, 2016
 *      Author: max
 */

#ifndef SRC_PHTREE_H_
#define SRC_PHTREE_H_

#include <iostream>

class Entry;
class Node;

class PHTree {
public:
	PHTree(int dim, int valueLength);
	virtual ~PHTree();
	void insert(Entry* e);
	bool lookup(Entry* e);
	friend std::ostream& operator<<(std::ostream& os, const PHTree& tree);

protected:
	Node* root_;
	size_t dim_; // dimensions (k)
	size_t valueLength_;
};

#endif /* SRC_PHTREE_H_ */
