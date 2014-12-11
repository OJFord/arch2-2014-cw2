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
	// constructs from address
	CacheAddress(unsigned,
				 unsigned, unsigned, unsigned, unsigned);
	
	// constructs from tag, index, offset
	CacheAddress(unsigned, unsigned, unsigned,
				 unsigned, unsigned, unsigned, unsigned);
	
	// instances are callable
	//	returns address
	unsigned operator()(void) const;
	
	// sets tag, leaving index and offset intact
	void tag(unsigned);
	
	// getters for fields within address
	unsigned tag(void) const;
	unsigned idx(void) const;
	unsigned offset(void) const;
	
protected:
	unsigned len_ofst;
	unsigned len_idx;
	unsigned len_tag;
	
private:
	unsigned _ofst;
	unsigned _idx;
	unsigned _tag;
};


// forward declaration of Cache
template< template<class> class C >
class Cache;

class CacheBlock{
public:
	CacheBlock(unsigned, unsigned);
	~CacheBlock(void);
	
	// gets data in word at given offset to buffer
	void get(uint8_t*, unsigned) const;
	// gets all data in block
	void get(uint8_t*) const;
	
	// sets data in word at given offset from buffer
	void set(unsigned, uint8_t*);
	// sets all data in block from buffer
	void set(uint8_t*);
	
	// loads a block from memory with given tag from buffer
	void load_mem(unsigned, uint8_t*);
	
	// getters for tag field, valid; dirty bits
	unsigned tag(void) const;
	bool valid(void) const;
	bool dirty(void) const;

private:
	fvec<Word>	words;
	
	const unsigned	wordSize;

	unsigned	_tag;
	bool		_valid;
	bool		_dirty;
	
	// friend flush function to allow clear of dirty bit
	template< template<class> class C >
	friend void Cache<C>::flush(void);
};

template< template<class> class C >
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
	
	// ADT to allow implementation of any compatible block replacement policy.
	//	('compatible' is still quite restricting - unknown if used for read
	//	or write, for example).
	//	mem_sim_queue.h
	queue<unsigned>*	idxq;
	
	const unsigned	blockSize;
	const unsigned	wordSize;
	
	// allow flush, debug to access all blocks by index, rather than tag
	friend void Cache<C>::flush(void);
	friend std::ostream& Cache<C>::debug(std::ostream&) const;
};

typedef enum{
	READ=0,
	WRITE=1
} rwMode;

template< template<class> class C >
class Cache: public MemoryLevel{
public:
	Cache(Ram*,		unsigned, unsigned,
		  unsigned, unsigned, unsigned,
		  unsigned, unsigned, unsigned);
	
	//	implements MemoryLevel read/write
	virtual void read(uint8_t*, unsigned);
	virtual void write(unsigned, uint8_t*);
	virtual void flush(void);
	
	std::ostream& debug(std::ostream&) const;
	
	bool		hit(void) const;
	unsigned	access_time(void) const;
	unsigned	set_idx(void) const;
	
protected:
	// read/write helper
	void rw(rwMode, uint8_t*, unsigned, bool=true);
	
private:
	fvec< CacheSet<C> >	sets;

	const unsigned	setSize;
	const unsigned	blockSize;
	const unsigned	wordSize;
	
	const unsigned	hit_mult;
	const unsigned	read_mult;
	const unsigned	write_mult;

	bool		_hit;
	unsigned	_access_time;
	unsigned	_set_idx;
};

// template class definitions
#include "mem_sim_cache.cpp"

#endif /* defined(__LRU_CacheSim__mem_sim_impl__) */
