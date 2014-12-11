//
//  mem_sim_mem_hier.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 07/12/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef LRU_CacheSim_mem_sim_mem_hier_h
#define LRU_CacheSim_mem_sim_mem_hier_h

#include <stdint.h>

class MemoryLevel{
public:
	MemoryLevel(MemoryLevel* lvl, unsigned alen)
	 : higher_mem(lvl), addressLen(alen){};
	virtual ~MemoryLevel(void){ delete higher_mem; };
	
	// reads word-length data from address into buffer
	virtual void read(uint8_t*, unsigned) = 0;
	
	// writes word-length data from buffer into address(+)
	virtual void write(unsigned, uint8_t*) = 0;
	
	// ensures consistency with higher_mem
	virtual void flush(void) = 0;
	
protected:
	MemoryLevel*	higher_mem;
	const unsigned	addressLen;
};

#endif
