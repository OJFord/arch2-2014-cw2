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
#include "mem_sim_fvec.h"

#define SizeMismatchException 2

unsigned log2(unsigned);

class cache_address{
public:
	cache_address(unsigned, unsigned, unsigned);
	
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
	virtual ~mem_level(void);
	
	virtual void read(uint8_t*, unsigned) = 0;
	virtual void write(unsigned, uint8_t*) = 0;

protected:
	mem_level* higher_mem;
};

class ram: public mem_level{
public:
	ram(unsigned, unsigned);
	~ram(void);
	
	virtual void read(uint8_t*, unsigned);
	virtual void write(unsigned, uint8_t*);
	
private:
	fvec<word>	words;
};

class cache_block{
public:
	cache_block(unsigned, unsigned);
	~cache_block(void);
	
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
	fvec<word>	words;
	
	const unsigned	wordSize;

	unsigned	_tag;
	bool		_valid;
	bool		_dirty;
};

class cache_set{
public:
	cache_set(unsigned, unsigned, unsigned);
	
	// locates block for a given tag
	//	returns block
	cache_block* get(unsigned);
	
	// sets a word within block for given tag, at given offset
	//	returns true if successful, false otherwise (tag not in set)
	bool set(unsigned, unsigned, uint8_t*);
	
	// loads a block from memory with specified tag
	//	returns evicted block value, or null block [not valid or dirty; params (0,0)]
	cache_block load(unsigned, uint8_t*);

private:
	fvec<cache_block>	blocks;
	lrque<unsigned>		lru;
	
	const unsigned	blockSize;
	const unsigned	wordSize;
};

typedef enum{
	READ=0,
	WRITE=1
} rwMode;

class cache: public mem_level{
public:
	cache(ram*, unsigned, unsigned, unsigned, unsigned);
	
	void rw(rwMode, uint8_t*, unsigned);
	virtual void read(uint8_t*, unsigned);
	virtual void write(unsigned, uint8_t*);
	
	bool		hit(void) const;
	unsigned	access_time(void) const;
	unsigned	set_idx(void) const;

private:
	fvec<cache_set>	sets;
	
	const unsigned	setSize;
	const unsigned	blockSize;
	const unsigned	wordSize;

	bool		_hit;
	unsigned	_access_time;
	unsigned	_set_idx;
};

#endif /* defined(__LRU_CacheSim__mem_sim_impl__) */
