//
//  mem_sim_impl.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 28/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef __LRU_CacheSim__mem_sim_impl__
#define __LRU_CacheSim__mem_sim_impl__

#include "mem_sim_lrque.h"
#include <vector>

unsigned log2(unsigned);

class cache_address{
public:
	cache_address(unsigned, unsigned, unsigned);
	
	unsigned get(void) const;
	void set(unsigned);

	unsigned tag(void) const;
	unsigned idx(void) const;
	unsigned offset(void) const;
	
private:
	unsigned _data;
	unsigned _tag;
	unsigned _idx;
	unsigned _offset;
};

class word{
public:
	word(unsigned, bool=false);
	word(unsigned, uint8_t*);
	~word(void);
	
	void set(fvec<uint8_t>);
	void set(word);
	fvec<uint8_t> get(void) const;
	
private:
	fvec<uint8_t> bytes;
};

class mem_level{
public:
	mem_level(mem_level*);
	
	virtual void read(uint8_t*, unsigned) const = 0;
	virtual void write(unsigned, uint8_t*) = 0;

protected:
	mem_level* higher_mem;
};

class ram: public mem_level{
public:
	ram(unsigned*, unsigned*);
	
	virtual void read(uint8_t*, unsigned) const;
	virtual void write(unsigned, uint8_t*);
	
private:
	fvec<word>	words;
};

class cache_block{
public:
	cache_block(unsigned*, unsigned*);
	
	void get(uint8_t*, unsigned*) const;
	void set(uint8_t*);
	void setFromMem(unsigned, uint8_t*);
	
	unsigned tag(void) const;
	bool valid(void) const;
	bool dirty(void) const;

private:
	fvec<word>	words;

	unsigned	_tag;
	bool		_valid;
	bool		_dirty;
};

class cache_set{
public:
	cache_set(unsigned*, unsigned*, unsigned*);
	
	cache_block get(unsigned);
	cache_block set(unsigned, uint8_t*);

private:
	std::vector<cache_block> blocks;
	lrque<unsigned> lru;
};

class cache: public mem_level{
public:
	cache(ram, unsigned, unsigned, unsigned, unsigned);
	
	virtual void read(uint8_t*, unsigned) const;
	virtual void write(unsigned, uint8_t*);

private:
	std::vector<cache_set>	sets;
	const unsigned	size;
	const unsigned	setSize;
	const unsigned	blockSize;
	const unsigned	wordSize;
};

#endif /* defined(__LRU_CacheSim__mem_sim_impl__) */
