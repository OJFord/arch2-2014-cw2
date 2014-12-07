//
//  mem_sim_impl.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 28/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#include "mem_sim_cache.h"

/*
 *	Cache block
*/

CacheBlock::CacheBlock(unsigned size, unsigned wLen)
 : wordSize(wLen), words(size, wLen){
	_valid	= false;
}

CacheBlock::~CacheBlock(void){
}

void CacheBlock::get(uint8_t* obuf, unsigned offset) const{
	fvec<uint8_t> word(wordSize);
	word = words.at( offset%wordSize ).get();					// get word at offset
	for(unsigned i=0; i<word.size(); ++i)
		obuf[i] = word.at(i);									// copy bytes
}

void CacheBlock::get(uint8_t* obuf) const{
	for(unsigned i=0; i<words.size(); ++i)
		get(obuf+i*words.size(), i);							// get each word
}

void CacheBlock::set(unsigned offset, uint8_t* ibuf){
	words.at( offset%wordSize ).set( Word( wordSize, ibuf ) );	// set word at offset
	_dirty = true;
}

void CacheBlock::set(uint8_t* ibuf){
	for(unsigned i=0; i<words.size(); ++i)
		set(i, ibuf+i*words.size());							// set each word
}

void CacheBlock::load_mem(unsigned tag, uint8_t* ibuf){
	set(ibuf);
	_tag = tag;
	_dirty = false;
	_valid = true;
}

unsigned CacheBlock::tag(void) const{
	return _tag;
}

bool CacheBlock::valid(void) const{
	return _valid;
}

bool CacheBlock::dirty(void) const{
	return _dirty;
}

/*
 *	Cache set
*/

CacheSet::CacheSet(unsigned setSize, unsigned blockSize, unsigned wordSize)
 :	blockSize(blockSize), wordSize(wordSize), blocks(setSize, blockSize, wordSize){
	for(unsigned i=0; i<setSize; ++i)
		lru.push(i);									// init LRU stack
}

CacheBlock* CacheSet::get(unsigned tag){
	for(unsigned i=0; i<blocks.size(); ++i){			// each block
		CacheBlock& cand = blocks.at(i);
		
		if( cand.valid() && cand.tag() == tag ){		// hit
			lru.repush(i);
			return &cand;
		}
	}
	return nullptr;										// miss
}

bool CacheSet::set(unsigned tag, unsigned offset, uint8_t* data){
	for(unsigned i=0; i<blocks.size(); ++i){			// each block
		CacheBlock& cand = blocks.at(i);
		
		if( cand.tag() == tag ){						// hit: block already in set
			cand.set(offset, data);
			lru.repush(i);
			return true;
		}
	}
	return false;										// miss
}

CacheBlock CacheSet::load(unsigned tag, uint8_t* ibuf){
	unsigned evict_idx = lru.pop();
	CacheBlock ret = blocks.at( evict_idx );			// grab block before it's kicked out
	
	blocks.at( evict_idx ).load_mem(tag, ibuf);
	return ret;											// return old block for write back
}

/*
 *	Cache
*/

Cache::Cache(Ram* mem, unsigned size, unsigned setSize,  unsigned blockSize, unsigned wordSize)
 :	MemoryLevel(mem),
	setSize(setSize),
	blockSize(blockSize),
	wordSize(wordSize),
	sets(size, setSize, blockSize, wordSize){
}

void Cache::rw(rwMode mode, uint8_t* buf, unsigned addr){
	
	CacheAddress address(addr, (unsigned)sets.size(), blockSize);
	_set_idx = address.idx()%sets.size();
	
	unsigned tag = address.tag();
	unsigned ofst= address.offset();

	CacheSet& set = sets.at(_set_idx);
	CacheBlock* cand = set.get(tag);					// set.get, lol
	
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
		
		CacheBlock evicted = set.load(tag, membuf);
		set.set(tag, ofst, buf);						// set.set, aarrgh
		
		if( evicted.valid() && evicted.dirty() ){		// write back
			uint8_t tmp[wordSize*blockSize];
			evicted.get(tmp);
			higher_mem->write(addr, tmp);
		}
	}
	
}

void Cache::read(uint8_t* obuf, unsigned addr){
	Cache::rw(READ, obuf, addr);
}

void Cache::write(unsigned addr, uint8_t* ibuf){
	Cache::rw(WRITE, ibuf, addr);
}

bool Cache::hit(void) const{
	return _hit;
}

unsigned Cache::access_time(void) const{
	return _access_time;
}

unsigned Cache::set_idx(void) const{
	return _set_idx;
}

/*
 * Cache address
*/

//	Integer log
unsigned log2(unsigned x){
	unsigned n=0;
	while(x != 0){
		n++;
		x >>= 1;
	}
	return n;
}

CacheAddress::CacheAddress(unsigned addr, unsigned numSets, unsigned wordsPerBlock): _data(addr){
	
	_offset = addr & ~(0xFFFFFFFF<<log2(wordsPerBlock));
	
	_idx = addr>>log2(_offset);							// remove offset
	_idx &= ~(0xFFFFFFFF<<log2(numSets));				// remove tag
	
	_tag = addr>>log2(_offset);							// remove offset
	_tag >>= log2(numSets);								// remove index

}

unsigned CacheAddress::addr(void) const{
	return _data;
}

unsigned CacheAddress::offset(void) const{
	return _offset;
}

unsigned CacheAddress::idx(void) const{
	return _idx;
}

unsigned CacheAddress::tag(void) const{
	return _tag;
}
