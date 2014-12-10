//
//  mem_sim_exceptions.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 07/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef LRU_CacheSim_mem_sim_exceptions_h
#define LRU_CacheSim_mem_sim_exceptions_h

class CacheException: public std::exception{
public:
	CacheException(const char* s): _what(s){};
	
	using std::exception::what;

protected:
	const char* _what;
};

class InvalidSizeException: public CacheException{
public:
	InvalidSizeException(const char* s): CacheException(s){};
};
class IncompatibleQueueException: public CacheException{
public:
	IncompatibleQueueException(const char* s): CacheException(s){};
};

#endif
