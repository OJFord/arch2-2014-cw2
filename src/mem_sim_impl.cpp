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
	//bytes = new fvec<uint8_t>(len, 0);
	if(zero)
		for(unsigned i=0; i<bytes.size(); ++i)
			bytes.at(i) = 0;
}

word::word(unsigned len, uint8_t* ibuf): bytes(len, 0){
	for(unsigned i=0; i<bytes.size(); ++i)
		bytes.at(i) = ibuf[i];
}

word::~word(void){
	//delete bytes;
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

cache_block::cache_block(unsigned size, unsigned wLen): words(size, wLen){
	_valid	= false;
}

cache_block::~cache_block(void){
}

void cache_block::get(uint8_t* obuf, unsigned offset) const{
	unsigned wlen = (unsigned)words.at(0).get().size();
	fvec<uint8_t> word(wlen);
	word = words.at( offset%wlen ).get();		// get word at offset
	for(unsigned i=0; i<word.size(); ++i)
		obuf[i] = word.at(i);							// copy bytes
}

void cache_block::get(uint8_t* obuf) const{
	for(unsigned i=0; i<words.size(); ++i)
		get(obuf+i*words.size(), i);					// get each word
}

void cache_block::set(unsigned offset, uint8_t* ibuf){
	unsigned wlen = (unsigned)words.at(0).get().size();
	words.at( offset%wlen ).set( word( wlen, ibuf ) );	// set word at offset
	_dirty = true;
}

void cache_block::set(uint8_t* ibuf){
	for(unsigned i=0; i<words.size(); ++i)
		set(i, ibuf+i*words.size());					// set each word
}

void cache_block::load_mem(unsigned tag, uint8_t* ibuf){
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
		lru.push(i);
	}
}

cache_block cache_set::get(unsigned tag){
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
		lru.repush(i);
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
		lru.repush(i);
		return cache_block(0, 0);			// null return
	}
	else{
		unsigned repl = lru.pop();
		cache_block old = blocks.at(repl);
		old.setFromMem(tag, data);
		lru.push(repl);
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
