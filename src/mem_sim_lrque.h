//
//  mem_sim_lrque.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 01/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef LRU_CacheSim_mem_sim_lrque_h
#define LRU_CacheSim_mem_sim_lrque_h

#include "mem_sim_queue.h"

template<class T>
class lrque: public queue<T>{
public:
	virtual void push(const T el);
	virtual T pop(void);
	virtual bool consume(const T);
};

template<class T>
void lrque<T>::push(const T el){
	std::deque<T>::push_back(el);
}

template<class T>
T lrque<T>::pop(void){
	T t = std::deque<T>::front();
	std::deque<T>::pop_front();
	return t;
}

template<class T>
bool lrque<T>::consume(const T el){
	for(unsigned i=0; i<std::deque<T>::size(); ++i){
		if( std::deque<T>::at(i) == el ){
			std::deque<T>::erase(this->begin()+i);
			push(el);
			return true;
		}
	}
	// el wasn't in queue
	return false;
}

#endif
