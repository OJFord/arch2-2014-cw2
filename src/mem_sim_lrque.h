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
	
	//	returns vector from begin (LRU) to end (MRU)
	virtual std::vector<T> dump(void) const;
};

template<class T>
void lrque<T>::push(const T el){
	queue<T>::push_back(el);
}

template<class T>
T lrque<T>::pop(void){
	T t = queue<T>::front();
	queue<T>::pop_front();
	return t;
}

template<class T>
bool lrque<T>::consume(const T el){
	for(unsigned i=0; i<queue<T>::size(); ++i){
		if( queue<T>::at(i) == el ){
			queue<T>::erase(queue<T>::begin()+i);
			push(el);
			return true;
		}
	}
	// el wasn't in queue
	return false;
}

template<class T>
std::vector<T> lrque<T>::dump(void) const{
	std::vector<T> tmp;
	for(unsigned i=0; i<queue<T>::size(); ++i)
		tmp.push_back( queue<T>::at(i) );
	return tmp;
}

#endif
