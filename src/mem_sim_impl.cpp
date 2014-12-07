//
//  mem_sim_impl.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 28/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#include "mem_sim_impl.h"

unsigned log2(unsigned x){
	unsigned n=0;
	while(x != 0){
		n++;
		x >>= 1;
	}
	return n;
}

mem_level::mem_level(mem_level* lvl): higher_mem(lvl){};
mem_level::~mem_level(void){
	delete higher_mem;
}

word::word(unsigned len, bool zero): bytes(len, 0){
	if(zero)
		for(unsigned i=0; i<bytes.size(); ++i)
			bytes.at(i) = 0;
}

word::word(unsigned len, uint8_t* ibuf): bytes(len, 0){
	for(unsigned i=0; i<bytes.size(); ++i)
		bytes.at(i) = ibuf[i];
}

word::~word(void){
}

fvec<uint8_t> word::get(void) const{
	return bytes;
}

void word::set(fvec<uint8_t> idata){
	if( idata.size() != bytes.size() )
		throw SizeMismatchException;

	bytes = idata;
}

void word::set(word copy){
	bytes = copy.bytes;
}

/*
 *	RAM
*/

ram::ram(unsigned size, unsigned wLen)
 : mem_level(nullptr), words(size, wLen, 0){}			// initialise mem to zeroes

ram::~ram(void){
	//delete words;
}

void ram::read(uint8_t* obuf, unsigned addr){
	fvec<uint8_t> word = words.at(addr).get();
	for(unsigned i=0; i<word.size(); ++i)
		obuf[i] = word.at(i);
}

void ram::write(unsigned addr, uint8_t* ibuf){
	unsigned wlen = (unsigned)words.at(0).get().size();
	words.at(addr).set( word(wlen, ibuf) );
}

/*
 *	Cache block
*/

cache_block::cache_block(unsigned size, unsigned wLen)
 : wordSize(wLen), words(size, wLen){
	_valid	= false;
}

cache_block::~cache_block(void){
}

void cache_block::get(uint8_t* obuf, unsigned offset) const{
	fvec<uint8_t> word(wordSize);
	word = words.at( offset%wordSize ).get();		// get word at offset
	for(unsigned i=0; i<word.size(); ++i)
		obuf[i] = word.at(i);							// copy bytes
}

void cache_block::get(uint8_t* obuf) const{
	for(unsigned i=0; i<words.size(); ++i)
		get(obuf+i*words.size(), i);					// get each word
}

void cache_block::set(unsigned offset, uint8_t* ibuf){
	words.at( offset%wordSize ).set( word( wordSize, ibuf ) );	// set word at offset
	_dirty = true;
}

void cache_block::set(uint8_t* ibuf){
	for(unsigned i=0; i<words.size(); ++i)
		set(i, ibuf+i*words.size());					// set each word
}

void cache_block::load_mem(unsigned tag, uint8_t* ibuf){
	set(ibuf);
	_tag = tag;
	_dirty = false;
	_valid = true;
}

unsigned cache_block::tag(void) const{
	return _tag;
}

bool cache_block::valid(void) const{
	return _valid;
}

bool cache_block::dirty(void) const{
	return _dirty;
}

/*
 *	Cache set
*/

cache_set::cache_set(unsigned setSize, unsigned blockSize, unsigned wordSize)
 :	blockSize(blockSize), wordSize(wordSize), blocks(setSize, blockSize, wordSize){
	for(unsigned i=0; i<setSize; ++i)
		lru.push(i);									// init LRU stack
}

cache_block* cache_set::get(unsigned tag){
	for(unsigned i=0; i<blocks.size(); ++i){			// each block
		cache_block& cand = blocks.at(i);
		
		if( cand.valid() && cand.tag() == tag ){		// hit
			lru.repush(i);
			return &cand;
		}
	}
	return nullptr;										// miss
}

bool cache_set::set(unsigned tag, unsigned offset, uint8_t* data){
	for(unsigned i=0; i<blocks.size(); ++i){			// each block
		cache_block& cand = blocks.at(i);
		
		if( cand.tag() == tag ){						// hit: block already in set
			cand.set(offset, data);
			lru.repush(i);
			return true;
		}
	}
	return false;										// miss
}

cache_block cache_set::load(unsigned tag, uint8_t* ibuf){
	unsigned evict_idx = lru.pop();
	cache_block ret = blocks.at( evict_idx );			// grab block before it's kicked out
	
	blocks.at( evict_idx ).load_mem(tag, ibuf);
	return ret;											// return old block for write back
}

/*
 *	Cache
*/

cache::cache(ram* mem, unsigned size, unsigned setSize,  unsigned blockSize, unsigned wordSize)
 :	mem_level(mem),
	setSize(setSize),
	blockSize(blockSize),
	wordSize(wordSize),
	sets(size, setSize, blockSize, wordSize){
}

void cache::rw(rwMode mode, uint8_t* buf, unsigned addr){
	
	cache_address address(addr, (unsigned)sets.size(), blockSize);
	_set_idx = address.idx()%sets.size();
	
	unsigned tag = address.tag();
	unsigned ofst= address.offset();

	cache_set& set = sets.at(_set_idx);
	cache_block* cand = set.get(tag);					// set.get, lol
	
	if( cand != NULL ){									// cache hit
		_hit = true;
		
		if( mode == READ )
			cand->get(buf, ofst);

		else if( mode == WRITE )
			cand->set(ofst, buf);
	}
	else{												// cache miss
		_hit = false;
		
		uint8_t* membuf = new uint8_t[ wordSize*blockSize ];
		for(unsigned i=0; i<blockSize; ++i)
			higher_mem->read(
				&membuf[i*wordSize],
				( addr - ofst ) | i						// read each word in block from mem
			);
		
		for(unsigned i=0; i<wordSize; ++i)
			if( mode == READ )
				buf[i] = ( &membuf[ofst*wordSize] )[i];	// copy each byte from word at offset
		
			else if( mode == WRITE )
				set.set(tag, ofst, buf);
		
		delete [] membuf;
		
		cache_block evicted = set.load(tag, membuf);
		set.set(tag, ofst, buf);						// set.set, aarrgh
		
		if( evicted.valid() && evicted.dirty() ){		// write back
			uint8_t tmp[wordSize*blockSize];
			evicted.get(tmp);
			higher_mem->write(addr, tmp);
		}
	}
	
}

void cache::read(uint8_t* obuf, unsigned addr){
	cache::rw(READ, obuf, addr);
}

void cache::write(unsigned addr, uint8_t* ibuf){
	cache::rw(WRITE, ibuf, addr);
}

bool cache::hit(void) const{
	return _hit;
}

unsigned cache::access_time(void) const{
	return _access_time;
}

unsigned cache::set_idx(void) const{
	return _set_idx;
}

/*
 * Cache address
*/

cache_address::cache_address(unsigned addr, unsigned numSets, unsigned wordsPerBlock): _data(addr){
	_offset = addr & ~(0xFFFFFFFF<<log2(wordsPerBlock));
	
	_idx = addr>>log2(_offset);							// remove offset
	_idx &= ~(0xFFFFFFFF<<log2(numSets));				// remove tag
	
	_tag = addr>>log2(_offset);							// remove offset
	_tag >>= log2(numSets);								// remove index

}

unsigned cache_address::addr(void) const{
	return _data;
}

unsigned cache_address::offset(void) const{
	return _offset;
}

unsigned cache_address::idx(void) const{
	return _idx;
}

unsigned cache_address::tag(void) const{
	return _tag;
}
