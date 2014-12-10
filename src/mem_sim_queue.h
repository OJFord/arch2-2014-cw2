//
//  mem_sim_queue.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 08/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef LRU_CacheSim_mem_sim_queue_h
#define LRU_CacheSim_mem_sim_queue_h

#include <deque>

template<class T>
class queue: protected std::deque<T>{
public:
	// pushes an item onto the queue
	virtual void push(T)	= 0;
	
	// removes an item from the queue
	//	returns removed item
	virtual T pop(void)		= 0;
	
	// Consumes somehow an index; may cause some action
	//	(e.g. a 're-push' as in LRU policy) or none
	//	returns true if item existed, false otherwise
	virtual bool consume(T)	= 0;
};

#endif
