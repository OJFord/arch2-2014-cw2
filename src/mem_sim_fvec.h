//
//  mem_sim_fvec.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 05/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef LRU_CacheSim_mem_sim_fvec_h
#define LRU_CacheSim_mem_sim_fvec_h

#include <vector>

/*
 *	Vector with length fixed on initialisation
*/
template<class T>
class fvec: private std::vector<T>{
public:
	template<class ...Args>
	fvec(unsigned size, Args&& ...args);
	~fvec(void);
	
	T at(unsigned) const;
	T& at(unsigned);
	
	unsigned long size(void) const;
};

template<class T>template<class ...Args>
fvec<T>::fvec(unsigned size, Args&& ...args){
	std::vector<T>::resize(size, T(args...));
};

template<class T>
fvec<T>::~fvec(void){
}

template<class T>
T fvec<T>::at(unsigned idx) const{
	return std::vector<T>::at(idx);
}

template<class T>
T& fvec<T>::at(unsigned idx){
	return std::vector<T>::at(idx);
}

template<class T>
unsigned long fvec<T>::size(void) const{
	return std::vector<T>::size();
}

#endif
