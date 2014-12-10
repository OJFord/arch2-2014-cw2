//
//  mem_sim_impl.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 28/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

//	ensure only brought in by header
//	with template class declarations
#ifdef __LRU_CacheSim__mem_sim_impl__

#include "mem_sim_exceptions.h"
#include "mem_sim_queue.h"

/*
 * Cache address
 */

//	Integer log
unsigned log2(unsigned x){
	unsigned n=-1;
	while(x != 0){
		n++;
		x >>= 1;
	}
	return n;
}

CacheAddress::CacheAddress( unsigned addr, unsigned addrLen, unsigned numSets,
						   unsigned wordsPerBlock, unsigned bytesPerWord ){
	
	len_ofst= log2(wordsPerBlock*bytesPerWord);
	len_idx	= log2(numSets);
	len_tag	= addrLen - ( len_idx + len_ofst );
	
	// mask address to correct len
	addr	&= ~( ((unsigned)-1) << (len_ofst + len_idx + len_tag) );
	
	_ofst = addr & ~( ((unsigned)-1) << len_ofst );
	_idx	= (addr & ~( ((unsigned)-1) << (len_ofst + len_idx) )) >> len_ofst;
	_tag	= addr >> (len_ofst + len_idx);
	
}

CacheAddress::CacheAddress( unsigned tag, unsigned idx, unsigned ofst,
						   unsigned addrLen, unsigned numSets, unsigned wordsPerBlock, unsigned bytesPerWord )
:	CacheAddress(0, addrLen, numSets, wordsPerBlock, bytesPerWord){
	_tag = tag;
	_idx = idx;
	_ofst= ofst;
}

void CacheAddress::tag(unsigned tag){
	_tag	=	tag;
}

unsigned CacheAddress::operator()(void) const{
	unsigned addr=0;
	addr |= _tag << (len_ofst+len_idx);
	addr |= _idx << len_ofst;
	addr |= _ofst;
	return addr;
}

unsigned CacheAddress::offset(void) const{
	return _ofst;
}

unsigned CacheAddress::idx(void) const{
	return _idx;
}

unsigned CacheAddress::tag(void) const{
	return _tag;
}

/*
 *	Cache block
*/

CacheBlock::CacheBlock(unsigned size, unsigned wLen)
 : words(size, wLen), wordSize(wLen){
	_valid	= false;
}

CacheBlock::~CacheBlock(void){
}

void CacheBlock::get(uint8_t* obuf, unsigned offset) const{
	fvec<uint8_t> word(wordSize);
	word = words.at( offset/wordSize ).get();				// get word at offset
	for(unsigned i=0; i<word.size(); ++i)
		obuf[i] = word.at(i);							// copy bytes
}

void CacheBlock::get(uint8_t* obuf) const{
	for(unsigned i=0; i<words.size(); ++i)
		get(obuf+i*words.size(), i);					// get each word
}

void CacheBlock::set(unsigned offset, uint8_t* ibuf){
	words.at(
		offset/wordSize
	).set( Word( wordSize, ibuf ) );					// set word at offset
	_dirty = true;
}

void CacheBlock::set(uint8_t* ibuf){
	for(unsigned i=0; i<words.size(); ++i)
		set(i*wordSize, ibuf+i*words.size());			// set each word
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

template< template<class> class C >
CacheSet<C>::CacheSet(unsigned setSize, unsigned blockSize, unsigned wordSize)
 :	blocks(setSize, blockSize, wordSize), blockSize(blockSize), wordSize(wordSize){
	
														// assert C is a queue
	if( !std::is_base_of< queue<unsigned>, C<unsigned> >::value )
		throw IncompatibleQueueException(
			"Template parameter must be class derived from queue."
		);
	
	idxq = new C<unsigned>;
	for(unsigned i=0; i<setSize; ++i)
		idxq->push(i);									// init LRU stack
}

template< template<class> class C >
CacheBlock* CacheSet<C>::get(unsigned tag){
	for(unsigned i=0; i<blocks.size(); ++i){			// each block
		CacheBlock& cand = blocks.at(i);
		
		if( cand.valid() && cand.tag() == tag ){		// hit
			idxq->consume(i);
			return &cand;
		}
	}
	return nullptr;										// miss
}

template< template<class> class C >
bool CacheSet<C>::set(unsigned tag, unsigned offset, uint8_t* data){
	CacheBlock* match = get(tag);
	if(match)											// hit
		match->set(offset, data);
	return match != NULL;
}

template< template<class> class C >
CacheBlock CacheSet<C>::load(unsigned tag, uint8_t* ibuf){
	unsigned evict_idx = idxq->pop();
	CacheBlock ret = blocks.at( evict_idx );			// grab block before it's kicked out
	
	blocks.at( evict_idx ).load_mem(tag, ibuf);
	idxq->push( evict_idx );
	return ret;											// return old block for write back
}

/*
 *	Cache
*/
template< template<class> class C >
Cache<C>::Cache(Ram* mem,	unsigned addrSize,	unsigned size,
	unsigned setSize,		unsigned blockSize,	unsigned wordSize,
	unsigned hitCycles,		unsigned readCycles,unsigned writeCycles)
:	MemoryLevel(mem, addrSize),
	sets(size, setSize, blockSize, wordSize),
	setSize(setSize),
	blockSize(blockSize),
	wordSize(wordSize),
	hit_mult(hitCycles),
	read_mult(readCycles),
	write_mult(writeCycles){
}

template< template<class> class C >
void Cache<C>::rw(rwMode mode, uint8_t* buf, unsigned addr, bool reset){
	
	CacheAddress	address(addr, addressLen, (unsigned)sets.size(), blockSize, wordSize);
	_set_idx		= address.idx()%sets.size();
	_access_time	= reset ? 0 : _access_time;			// reset time only if not second call
	
	unsigned tag	= address.tag();
	unsigned ofst	= address.offset();

	CacheSet<C>& set= sets.at(_set_idx);
	CacheBlock* cand= set.get(tag);						// set.get, ugh
	
	if( cand != NULL ){									// cache hit
		_hit = reset ? true : _hit;						// set to hit only if not second call
		_access_time += 1*hit_mult;
		
		if( mode == READ )
			cand->get(buf, ofst);

		else if( mode == WRITE )
			cand->set(ofst, buf);
	}
	else{												// cache miss
		_hit = false;
		_access_time += 1*read_mult;

		uint8_t* membuf = new uint8_t[ wordSize*blockSize ];
		for(unsigned i=0; i<blockSize; ++i)
			higher_mem->read(
				&membuf[i*wordSize],
				(addr - ofst) + i*wordSize				// read each word in block from mem
			);
		
		CacheBlock evicted = set.load(tag, membuf);
		delete[] membuf;
		
		if( evicted.valid() && evicted.dirty() ){		// write back
			uint8_t tmp[wordSize*blockSize];
			evicted.get(tmp);
			
			CacheAddress oldAddr( addr, addressLen,
				(unsigned)sets.size(), blockSize, wordSize);
			oldAddr.tag( evicted.tag() );
			
			higher_mem->write(oldAddr(), tmp);
			_access_time += 1*write_mult;
		}
		
		rw(mode, buf, addr, false);						// go again for hit, no time/hit reset
		
	}
	
}

template< template<class> class C >
void Cache<C>::read(uint8_t* obuf, unsigned addr){
	Cache::rw(READ, obuf, addr);
}

template< template<class> class C >
void Cache<C>::write(unsigned addr, uint8_t* ibuf){
	Cache<C>::rw(WRITE, ibuf, addr);
}

template< template<class> class C >
void Cache<C>::flush(void){
	
	for(unsigned i=0; i<sets.size(); ++i){
		CacheSet<C>& set = sets.at(i);
		
		for(unsigned j=0; j<setSize; ++j){
			CacheBlock& block = set.blocks.at(j);
			
			if( block.valid() && block.dirty() ){
				uint8_t buf[ blockSize*wordSize ];
				block.get(buf);
				CacheAddress addr(	block.tag(), i, 0,
					addressLen, (unsigned)sets.size(), blockSize, wordSize );
				higher_mem->write(addr(), buf);
				block._dirty = false;
			}
		}
	}
}

template< template<class> class C >
std::ostream& Cache<C>::debug(std::ostream& os) const{
	
	os << "#" << std::endl << "# Cache Data : {" << std::endl;
	
	for(unsigned i=0; i<sets.size(); ++i){
		const CacheSet<C>& set = sets.at(i);
		os << "# \tS" << i << " : {" << std::endl;
		
		for(unsigned j=0; j<setSize; ++j){
			const CacheBlock& block = set.blocks.at(j);
			unsigned blockNum = ( block.tag() << log2( (unsigned)sets.size() ) ) | i;
			os << "# \t\tB" << blockNum << " : {" << std::endl;

			for(unsigned k=0; k<blockSize; ++k){
				const Word& word = block.words.at(k);
				os << "# \t\t\t0x";
				
				for(unsigned l=0; l<wordSize; ++l)
					os << std::hex << (unsigned)word.get().at(l);
				os << (k+1<sets.size() ? "," : "") << std::endl;
			}
			os << "# \t\t}" << (j+1<sets.size() ? "," : "") << std::endl;
		}
		os << "# \t}" << (i+1<sets.size() ? "," : "") << std::endl;
	}
	
	os << "#" << std::endl << "# Block Replace Queue : {" << std::endl;
	
	for(unsigned i=0; i<sets.size(); ++i){
		const CacheSet<C>& set = sets.at(i);
		os << "# \tS" << i << " : { " << std::endl;
		
		std::vector<unsigned> queue = set.idxq->dump();
		for(unsigned j=0; j<setSize; ++j){
			os << "# \t\tS" << i << "[" << queue.at(j) << "]";
			os << (j+1<setSize ? ", " : "") << std::endl;
		}
		os << "# \t}" << std::endl;
	}
	
	return os << "# }" << std::endl << "#" << std::endl;
}


template< template<class> class C >
bool Cache<C>::hit(void) const{
	return _hit;
}

template< template<class> class C >
unsigned Cache<C>::access_time(void) const{
	return _access_time;
}

template< template<class> class C >
unsigned Cache<C>::set_idx(void) const{
	return _set_idx;
}

#endif	/* defined(__LRU_CacheSim__mem_sim_impl__) */
