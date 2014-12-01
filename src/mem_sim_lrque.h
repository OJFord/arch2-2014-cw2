//
//  mem_sim_lrque.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 01/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef LRU_CacheSim_mem_sim_lrque_h
#define LRU_CacheSim_mem_sim_lrque_h

#include <deque>

template<class T> class lrque: private std::deque<T>{
public:
	//Push new item to queue
	void push(const T el);
	
	//Pop item from queue
	T pop(void);
	
	//Push existing item to the back of the queue
	//	returns false if item didn't exist, true otherwise
	bool repush(const T);
};

template<class T> void lrque<T>::push(const T el){
	this->push_back(el);
}

template<class T> T lrque<T>::pop(void){
	T t = this->front();
	this->pop_front();
	return t;
}

template<class T> bool lrque<T>::repush(const T el){
	for(unsigned i=0; i<this->size(); ++i){
		if( this->at(i) == el ){
			this->erase(this->begin()+i);
			this->push(el);
			return true;
		}
	}
	// el wasn't in queue
	return false;
}

#endif
