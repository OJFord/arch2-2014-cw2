//
//  mem_sim_impl.h
//  LRU-CacheSim
//
//  Created by Ollie Ford on 28/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#ifndef __LRU_CacheSim__mem_sim_impl__
#define __LRU_CacheSim__mem_sim_impl__

#include "mem_sim_mem_hier.h"
#include "mem_sim_lrque.h"
#include "mem_sim_fvec.h"

#include "mem_sim_ram.h"
#include "mem_sim_word.h"

class CacheAddress{
public:
	CacheAddress(unsigned, unsigned, unsigned, unsigned, unsigned);
	
	unsigned get(void) const;
	void set(unsigned);

	unsigned addr(void) const;
	unsigned tag(void) const;
	unsigned idx(void) const;
	unsigned offset(void) const;
	
private:
	unsigned _data;
	unsigned _tag;
	unsigned _idx;
	unsigned _offset;
};

class CacheBlock{
public:
	CacheBlock(unsigned, unsigned);
	~CacheBlock(void);
	
	//gets data in word at given offset to buffer
	void get(uint8_t*, unsigned) const;
	// gets all data in block
	void get(uint8_t*) const;
	
	// sets data in word at given offset from buffer
	void set(unsigned, uint8_t*);
	// sets all data in block from buffer
	void set(uint8_t*);
	
	// loads a block from memory with given tag from buffer
	void load_mem(unsigned, uint8_t*);
	
	unsigned tag(void) const;
	bool valid(void) const;
	bool dirty(void) const;

private:
	fvec<Word>	words;
	
	const unsigned	wordSize;

	unsigned	_tag;
	bool		_valid;
	bool		_dirty;
};

class CacheSet{
public:
	CacheSet(unsigned, unsigned, unsigned);
	
	// locates block for a given tag
	//	returns block
	CacheBlock* get(unsigned);
	
	// sets a word within block for given tag, at given offset
	//	returns true if successful, false otherwise (tag not in set)
	bool set(unsigned, unsigned, uint8_t*);
	
	// loads a block from memory with specified tag
	//	returns evicted block value, or null block [not valid or dirty; params (0,0)]
	CacheBlock load(unsigned, uint8_t*);

private:
	fvec<CacheBlock>	blocks;
	lrque<unsigned>		lru;
	
	const unsigned	blockSize;
	const unsigned	wordSize;
};

typedef enum{
	READ=0,
	WRITE=1
} rwMode;

class Cache: public MemoryLevel{
public:
	Cache(Ram*, unsigned, unsigned, unsigned, unsigned, unsigned);
	
	//	Implements MemoryLevel read/write
	virtual void read(uint8_t*, unsigned);
	virtual void write(unsigned, uint8_t*);
	
	bool		hit(void) const;
	unsigned	access_time(void) const;
	unsigned	set_idx(void) const;
	
protected:
	// Read/write helper
	void rw(rwMode, uint8_t*, unsigned);
	
private:
	fvec<CacheSet>	sets;

	const unsigned	setSize;
	const unsigned	blockSize;
	const unsigned	wordSize;

	bool		_hit;
	unsigned	_access_time;
	unsigned	_set_idx;
};

#endif /* defined(__LRU_CacheSim__mem_sim_impl__) */
