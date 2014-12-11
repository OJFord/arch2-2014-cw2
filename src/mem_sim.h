//
//  mem_sim.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 27/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef LRU_CacheSim_mem_sim_h
#define LRU_CacheSim_mem_sim_h

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#include "mem_sim_cache.h"

template< template<class> class C >
void read(Cache<C>* cache, unsigned wlen, std::stringstream params){
	std::string s, serr;
	params >> s;
	
	if( params >> serr ){
		std::cerr << "Unexpected extra parameter to write-req: " << serr;
		exit( EXIT_FAILURE );
	}
	
	unsigned addr = std::stoi(s);
	
#ifdef DEBUG
	std::cout << std::showbase << std::hex;
	std::cout << "# Reading " << addr << std::endl;
	std::cout << std::noshowbase;
#endif
	
	uint8_t buf[wlen];
	
	cache->read(buf, addr);
	unsigned data = buf[0];
	for(unsigned i=1; i<wlen; ++i){
		data <<= 8;
		data |= buf[i];
	}
	
	std::cout
	<< "read-ack"
	<< " " << std::hex << std::uppercase << std::setfill('0') << std::setw(wlen*2) << data
	<< " " << cache->set_idx()
	<< " " << (cache->hit() ? "hit" : "miss")
	<< " " << cache->access_time()
	<< std::endl;
}

template< template<class> class C >
void write(Cache<C>* cache, unsigned wlen, std::stringstream params){
	
	std::string s1, s2, serr;
	unsigned addr;
	uint8_t data[wlen];
	
	params >> s1 >> s2;
	
	if( params >> serr ){
		std::cerr << "Unexpected extra parameter to write-req: " << serr;
		exit( EXIT_FAILURE );
	}
	
	addr	= std::stoi(s1, nullptr, 10);
	
	// assumes leading zeroes will be present if applicable
	for(unsigned i=0; i<wlen; ++i)
		data[i] = std::stoi( s2.substr(i*2, 2), nullptr, 16 );
	
#ifdef DEBUG
	std::cout << "# Writing ";
	for(unsigned i=0; i<wlen; ++i)
		std::cout << std::setfill('0') << std::setw(2) << (int)data[i];
	
	std::cout << std::showbase;
	std::cout << " to " << addr << std::endl;
	std::cout << std::noshowbase;
#endif
	
	cache->write(addr, data);
	
	std::cout
	<< "write-ack"
	<< " " << cache->set_idx()
	<< " " << (cache->hit() ? "hit" : "miss")
	<< " " << cache->access_time()
	<< std::endl;
}

template< template<class> class C >
void flush(Cache<C>* cache){
	cache->flush();
	std::cout << "flush-ack " << cache->access_time() << std::endl;
}

template< template<class> class C >
void debug(Cache<C>* cache){
	std::cout << "debug-ack-begin" << std::endl;
	cache->debug(std::cout);
	std::cout << "debug-ack-end" << std::endl;
}

#endif
