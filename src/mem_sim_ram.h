//
//  mem_sim_ram.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 07/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef LRU_CacheSim_mem_sim_ram_h
#define LRU_CacheSim_mem_sim_ram_h

#include "mem_sim_mem_hier.h"
#include "mem_sim_fvec.h"
#include "mem_sim_word.h"

class Ram: public MemoryLevel{
public:
	Ram(unsigned, unsigned);
	~Ram(void);
	
	virtual void read(uint8_t*, unsigned);
	virtual void write(unsigned, uint8_t*);
	
private:
	fvec<Word>	words;
};


#endif
