//
//  mem_sim_impl.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 28/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef __LRU_CacheSim__mem_sim_impl__
#define __LRU_CacheSim__mem_sim_impl__

#include <stack>

class mem{
public:
	mem(void);
	
	void write(uint8_t*, unsigned);
	void read(uint8_t*, unsigned);

private:
};

class word{
public:
	word(unsigned);
	
	uint8_t* bytes;
	unsigned size;
};

class cache_block{
public:
	cache_block(unsigned);

private:
	word* words;
	unsigned size;
};

class cache_set{
public:
	cache_set(unsigned);

private:
	std::stack<cache_block> blocks;
	unsigned size;
};

class cache{
public:
	cache(void);
	
	void write(uint8_t*, unsigned);
	void read(uint8_t*, unsigned);

private:
	cache_set*	sets;
	unsigned	size;
	
	mem* higher_mem;
};

#endif /* defined(__LRU_CacheSim__mem_sim_impl__) */
