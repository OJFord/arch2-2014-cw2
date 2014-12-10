//
//  mem_sim_word.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 07/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef __LRU_CacheSim__mem_sim_word__
#define __LRU_CacheSim__mem_sim_word__

#include "mem_sim_fvec.h"

class Word: private fvec<uint8_t>{
public:
	// optionally zeroes the constructed word
	Word(unsigned, bool=false);
	
	// constructs from word-sized buffer
	Word(unsigned, uint8_t*);

	~Word(void);
	
	// sets Word from fvec
	void set(fvec<uint8_t>);
	
	// copy setter
	void set(Word);
	
	// getter
	fvec<uint8_t> get(void) const;
};

#endif /* defined(__LRU_CacheSim__mem_sim_word__) */
