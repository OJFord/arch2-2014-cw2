//
//  mem_sim_word.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 07/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#include "mem_sim_word.h"
#include "mem_sim_exceptions.h"

Word::Word(unsigned len, bool zero): bytes(len, 0){
	if(zero)
		for(unsigned i=0; i<bytes.size(); ++i)
			bytes.at(i) = 0;
}
Word::Word(unsigned len, uint8_t* ibuf): bytes(len, 0){
	for(unsigned i=0; i<bytes.size(); ++i)
		bytes.at(i) = ibuf[i];
}

Word::~Word(void){
}

fvec<uint8_t> Word::get(void) const{
	return bytes;
}

void Word::set(fvec<uint8_t> idata){
	if( idata.size() != bytes.size() )
		throw SizeMismatchException;
	
	bytes = idata;
}

void Word::set(Word copy){
	bytes = copy.bytes;
}
