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
	Word(unsigned, bool=false);
	Word(unsigned, uint8_t*);
	~Word(void);
	
	void set(fvec<uint8_t>);
	void set(Word);
	fvec<uint8_t> get(void) const;
	
private:
	fvec<uint8_t> bytes;
};

#endif /* defined(__LRU_CacheSim__mem_sim_word__) */
