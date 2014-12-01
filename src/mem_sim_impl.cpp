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

ram::ram(unsigned* size, unsigned* wLen): mem_level(nullptr), size(size), wLen(wLen){
	bytes = new uint8_t[*wLen**size];
}

void ram::read(uint8_t* obuf, unsigned addr) const{
	for(unsigned i=0; i<*wLen; ++i)
		*obuf++ = bytes[addr+i];
}

void ram::write(unsigned addr, uint8_t* ibuf){
	for(unsigned i=0; i<*wLen; ++i)
		bytes[addr+i] = *ibuf++;
}

cache_block::cache_block(unsigned* size, unsigned* wLen): size(size), wLen(wLen){
	bytes	= new uint8_t[*wLen**size];
	_valid	= false;
}

void cache_block::get(uint8_t* obuf, unsigned* offset) const{
	if( offset == nullptr ){
		// all words
		for(unsigned i=0; i<(*wLen)*(*size); ++i)
			*obuf++ = bytes[i];
	}
	else{
		// word at offset
		for(unsigned i=0; i<*wLen; ++i)
			*obuf++ = *(bytes+(*offset)*(*size)+i);
	}
}

void cache_block::set(uint8_t* ibuf){
	for(unsigned i=0; i<(*wLen)*(*size); ++i)
		bytes[i] = *ibuf++;
	_dirty = true;
}

void cache_block::setFromMem(unsigned tag, uint8_t* ibuf){
	_tag = tag;
	set(ibuf);
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

cache_set::cache_set(unsigned* setSize, unsigned* blockSize, unsigned* wordSize): size(setSize){
	blocks.reserve(*setSize);
	for(unsigned i=0; i<*setSize; ++i){
		blocks.at(i) = cache_block(blockSize, wordSize);
		lru.push( blocks.at(i) );
	}
}

cache_block cache_set::get(unsigned tag) const{
	cache_block* match = nullptr;
	
	unsigned i=0;
	for(; i<*size; ++i){					// each block
		cache_block cand = blocks.at(i);
		if( cand.tag() == tag ){
			match = &cand;
			break;
		}
	}
	
	if( match != NULL ){					// hit
		lru.erase(i);
		lru.push_back(&match);
		return *match;
	}
	else									// miss
		return cache_block(0, 0);			// null return
}

cache_block cache_set::set(unsigned tag, uint8_t* data){
	cache_block* match = nullptr;
	
	unsigned i=0;
	for(; i<*size; ++i){					// each block
		cache_block cand = blocks.at(i);
		if( cand.tag() == tag ){
			match = &cand;
			break;
		}
	}
	
	if( match != NULL ){
		match->set(data);
		lru.erase(i);
		lru.push_back(&match);
		return cache_block(0, 0);			// null return
	}
	else{
		cache_block old = lru.pop_front();
		old.setFromMem(tag, data);
		lru.push_back(old);
		return old;							// return old for write back
	}
}

cache::cache(ram mem, unsigned size, unsigned setSize, unsigned blockSize, unsigned wordSize)
: mem_level(mem), size(size), setSize(setSize), blockSize(blockSize), wordSize(wordSize){
	
	sets.reserve(size);
	for(unsigned i=0; i<size; ++i)
		sets.at(i) = cache_set(&setSize, &blockSize, &wordSize);
}

void cache::read(uint8_t* obuf, unsigned addr) const{
	cache_address address(addr, setSize, size);
	unsigned idx = address.idx();
	
	cache_set set = sets.at( idx%size );
	cache_block* match = nullptr;
	
	for(unsigned i=0; i<size; ++i){			// each block in set
		cache_block cand = set.get(i);		// set.get, lol
		
		// candidate block is valid and tag matches
		if( cand.valid() && cand.tag() == address.tag() ){
			match = &cand;
			break;
		}
	}
	
	if( match != NULL ){
		// cache hit
		unsigned offset = address.offset();
		match->get(obuf, &offset);
	}
	else{
		// cache miss
		higher_mem->read(obuf, addr);
		cache_block old = set.set(address.tag(), obuf);	// set.set, aarrgh
		
		if( old.valid() && old.dirty() ){	// write back
			uint8_t buf[wordSize*blockSize];
			old.get(buf, nullptr);
			higher_mem->write(addr, buf);
		}
	}
	
}

cache_address::cache_address(unsigned addr, unsigned numBlocks, unsigned numSets): _data(addr){
	_offset = addr & ~(0xFFFFFFFF<<log2(numBlocks));
	
	_idx = addr>>log2(_offset);				// remove offset
	_idx &= ~(0xFFFFFFFF<<log2(numSets));	// remove tag
	
	_tag = addr>>log2(_offset);				// remove offset
	_tag >>= log2(numSets);					// remove index

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
