//
//  mem_sim_ram.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 07/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#include "mem_sim_ram.h"

/*
 *	RAM
*/

Ram::Ram(unsigned size, unsigned wLen)
: MemoryLevel(nullptr), words(size, wLen, 0){}		// initialise mem to zeroes

Ram::~Ram(void){
}

void Ram::read(uint8_t* obuf, unsigned addr){
	fvec<uint8_t> word = words.at(addr).get();
	for(unsigned i=0; i<word.size(); ++i)
		obuf[i] = word.at(i);
}

void Ram::write(unsigned addr, uint8_t* ibuf){
	unsigned wlen = (unsigned)words.at(0).get().size();
	words.at(addr).set( Word(wlen, ibuf) );
}
