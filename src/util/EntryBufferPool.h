/*
 * EntryBufferPool.h
 *
 *  Created on: Jul 13, 2016
 *      Author: max
 */

#ifndef SRC_UTIL_ENTRYBUFFERPOOL_H_
#define SRC_UTIL_ENTRYBUFFERPOOL_H_

#include <mutex>

template <unsigned int DIM, unsigned int WIDTH>
class EntryBuffer;

template <unsigned int DIM, unsigned int WIDTH>
class EntryBufferPool {
public:
	EntryBufferPool();
	~EntryBufferPool() {};

	// allocate must only be called by one thread at a time
	EntryBuffer<DIM, WIDTH>* allocate();
	// deallocate can be called by any number of threads at a time
	void deallocate(EntryBuffer<DIM, WIDTH>* buffer);
	// deallocates all contents assuming nobody can interfere (synchronized flush phase)
	void fullDeallocate();

	static const size_t capacity_ = 1000;
private:
	size_t headIndex_;
	size_t nInitialized_;
	size_t nFree_;
	mutex singleOperationMutex_;
	EntryBuffer<DIM, WIDTH> pool_[capacity_];

	bool assertClearedFreeList();
};

#include <assert.h>
#include <thread>
#include "util/EntryBuffer.h"
#include "util/DynamicNodeOperationsUtil.h"

template <unsigned int DIM, unsigned int WIDTH>
EntryBufferPool<DIM, WIDTH>::EntryBufferPool() :
	headIndex_(0), nInitialized_(0), nFree_(capacity_),
	singleOperationMutex_(), pool_() {
#ifndef NDEBUG
	// Validate that all buffers are cleared
	for (unsigned i = 0; i < capacity_; ++i) {
		pool_[i].assertCleared();
	}
#endif
}

template <unsigned int DIM, unsigned int WIDTH>
bool EntryBufferPool<DIM, WIDTH>::assertClearedFreeList() {
	// Validate that free list is cleared
	size_t currentIndex = headIndex_;
	while (currentIndex != (-1u) && currentIndex < nInitialized_) {
		const size_t nextIndex = pool_[currentIndex].nextIndex_;
		pool_[currentIndex].nextIndex_ = 0;
		pool_[currentIndex].assertCleared();
		pool_[currentIndex].nextIndex_ = nextIndex;
		currentIndex = nextIndex;
	}
	return true;
}

template <unsigned int DIM, unsigned int WIDTH>
void EntryBufferPool<DIM, WIDTH>::fullDeallocate() {
	bool locked = singleOperationMutex_.try_lock(); // TODO not needed!
	assert (locked);
	assert (assertClearedFreeList());

	// disables the current items in the free list by flagging their index
	// attention: this destroys the free list so reset() should be called afterwards!
	size_t nextIndex = headIndex_;
	headIndex_ = -1u;
	while (nextIndex != (-1u) && nextIndex < nInitialized_) {
		const size_t nextIndexTmp = pool_[nextIndex].nextIndex_;
		pool_[nextIndex].nextIndex_ = -1u;
		nextIndex = nextIndexTmp;
	}

	// go through the pool backwards and deallocate all buffers where the
	// assigned node can be locked
	for (unsigned i = 0; i < nInitialized_; ++i) {
		if (pool_[i].nextIndex_ != (-1u)) {
			DynamicNodeOperationsUtil<DIM, WIDTH>::flushSubtree(&(pool_[i]), false);
			assert (pool_[i].assertCleared());
		} else {
			pool_[i].nextIndex_ = 0; // TODO only needed for consistency in assertCleared()
			assert (pool_[i].assertCleared());
		}
	}

#ifndef NDEBUG
	// Validate that all buffers are now empty
	for (unsigned i = 0; i < capacity_; ++i) {
		pool_[i].assertCleared();
	}
#endif

	nFree_ = capacity_;
	headIndex_ = 0;
	nInitialized_ = 0;

	singleOperationMutex_.unlock();
}

template <unsigned int DIM, unsigned int WIDTH>
EntryBuffer<DIM, WIDTH>* EntryBufferPool<DIM, WIDTH>::allocate() {
	unique_lock<mutex> lk(singleOperationMutex_);
	assert (assertClearedFreeList());

	if (nInitialized_ < capacity_) {
		// not all fields were initialized so add the next entry to the free list
		assert (pool_[nInitialized_].assertCleared());
		pool_[nInitialized_].nextIndex_ = nInitialized_ + 1;
		pool_[nInitialized_].setPool(this);
		++nInitialized_;
	}

	EntryBuffer<DIM,WIDTH>* alloc = NULL;
	if (nFree_ > 0) {
		--nFree_;
		alloc = &pool_[headIndex_];
		if (nFree_ > 0) {
			headIndex_ = pool_[headIndex_].nextIndex_;
		} else {
			headIndex_ = -1u;
		}

		alloc->nextIndex_ = 0;
		assert (alloc->assertCleared());
	}

	return alloc;
}

template <unsigned int DIM, unsigned int WIDTH>
void EntryBufferPool<DIM, WIDTH>::deallocate(EntryBuffer<DIM, WIDTH>* buffer) {

	const size_t diff = (size_t)buffer - (size_t)pool_;
	const size_t slotSize = sizeof (EntryBuffer<DIM, WIDTH>);
	assert (diff % slotSize == 0);
	const size_t index = diff / slotSize;
	assert ((size_t)buffer >= (size_t)pool_ && index < capacity_);
	assert (buffer->assertCleared());

	unique_lock<mutex> lk(singleOperationMutex_);
	assert (assertClearedFreeList());

	if (headIndex_ != (-1u)) {
		buffer->nextIndex_ = headIndex_;
	} else {
		buffer->nextIndex_ = -1u;
	}

	headIndex_ = index;
	++nFree_;
}

#endif /* SRC_UTIL_ENTRYBUFFERPOOL_H_ */
