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

#include "mem_sim_cache.h"

void read(Cache*, unsigned, std::stringstream);
void write(Cache*, unsigned, std::stringstream);
void flush(Cache*);
void debug(Cache*);

#endif
