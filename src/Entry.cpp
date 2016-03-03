/*
 * Entry.cpp
 *
 *  Created on: Feb 25, 2016
 *      Author: max
 */

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

using namespace std;

#include "Entry.h"

Entry::Entry(vector<int> values, int bitLength) {
	values_.reserve(values.size());

	for (size_t i = 0; i < values.size(); i++) {
		vector<bool> value(bitLength);
		for (int j = 0; j < bitLength; j++) {
			// extract j-th least segnificant bit from int
			int lsbIndex = bitLength - j - 1;
			bool bit = ((values[i] & (1 << lsbIndex)) >> lsbIndex) == 1;
			value[j] = bit;
		}
		values_.push_back(value);
	}
}

Entry::Entry(vector<vector<bool>> values) {
	values_ = values;
}

Entry::~Entry() {
	// delete &values_;
}

ostream& operator <<(ostream& os, const Entry &e) {
	os << "(";
	for (size_t value = 0; value < e.values_.size(); value++) {
		assert (e.values_[value].size() == e.values_[0].size());
		os << "(";
		for (size_t bit = 0; bit < e.values_[value].size(); bit++) {
			int bitNumber = (e.values_[value][bit]) ? 1 : 0;
			os << bitNumber;
		}

		os << ")";
		if (value != e.values_.size() -1)
			os << ", ";
	}
	os << ")";

	return os;
}