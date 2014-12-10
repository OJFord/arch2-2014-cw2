//
//  mem_sim_word.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 07/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#include "mem_sim_word.h"
#include "mem_sim_exceptions.h"

Word::Word(unsigned len, bool zero): fvec<uint8_t>(len, 0){
	if(zero)
		for(unsigned i=0; i<fvec<uint8_t>::size(); ++i)
			fvec<uint8_t>::at(i) = 0;
}
Word::Word(unsigned len, uint8_t* ibuf): fvec<uint8_t>(len, 0){
	for(unsigned i=0; i<fvec<uint8_t>::size(); ++i)
		fvec<uint8_t>::at(i) = ibuf[i];
}

Word::~Word(void){
}

fvec<uint8_t> Word::get(void) const{
	return (fvec<uint8_t>)*this;
}

void Word::set(fvec<uint8_t> idata){
	if( idata.size() != fvec<uint8_t>::size() )
		throw SizeMismatchException;
	
	fvec<uint8_t>::operator=(idata);
}

void Word::set(Word copy){
	fvec<uint8_t>::operator=(copy);
}
