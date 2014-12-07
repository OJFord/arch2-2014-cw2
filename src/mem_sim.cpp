//
//  mem_sim.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 25/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#include "mem_sim.h"
#include <fstream>

int main(int argc, const char * argv[]){
	
#ifdef DEBUG		// pipe input file to stdin
	std::ifstream arq(getenv("SAMPLE_INPUT"));
	//std::cin.rdbuf(arq.rdbuf());
#endif
	
	unsigned addr, bpw, wpb, bps, spc, time_hit, time_write, time_read;
	
	if( argc < 8 ){
		std::cerr << "Insufficient parameters supplied." << std::endl;
		exit( EXIT_FAILURE );
	}
	
	addr= atoi( argv[1] );
	bpw	= atoi( argv[2] );
	wpb	= atoi( argv[3] );
	bps	= atoi( argv[4] );
	spc	= atoi( argv[5] );
	
	time_hit	= atoi( argv[6] );
	time_write	= atoi( argv[7] );
	time_read	= atoi( argv[8] );

	Ram*	ram	= new Ram(wpb, bpw);
	Cache*	cache = new Cache(ram, spc, bps, wpb, bpw);

	std::string input;
	while( std::getline(std::cin, input) ){
		
		if( input.compare(0, 1, "#") == 0 );
			//Comment. Ignore until newline.
		
		else if( input.compare(0, 9, "read-req ") == 0 )
			read( cache, bpw, std::stringstream(input.substr(9)) );
		
		else if( input.compare(0, 10, "write-req ") == 0)
			write( cache, bpw, std::stringstream(input.substr(9)) );
		
		else if( input.compare(0, 10, "flush-req ") == 0)
			flush( cache );
		
		else if( input.compare(0, 10, "debug-req ") == 0)
			debug( cache );
		
		else
			std::cerr << "Malformed input." << std::endl;
		
	}
	
	exit( EXIT_SUCCESS );
}

void read(Cache* cache, unsigned wlen, std::stringstream params){
	std::cout << "read-ack ";
	
	std::string s, serr;
	params >> s;
	
	if( params >> serr ){
		std::cerr << "Unexpected extra parameter to write-req: " << serr;
		exit( EXIT_FAILURE );
	}

	unsigned addr = std::stoi(s);

#ifdef DEBUG
	std::cout << std::endl << "Reading " << addr << std::endl;
#endif
	
	uint8_t buf[wlen];
	
	cache->read(buf, addr);
	unsigned data = buf[0];
	for(unsigned i=1; i<wlen; ++i){
		data <<= 8;
		data |= buf[i];
	}
	
	std::cout << " " << cache->set_idx() << " ";
	if( cache->hit() )
		std::cout << "hit ";
	else
		std::cout << "miss ";
	
	std::cout << cache->access_time() << " " << data << std::endl;
}

void write(Cache* cache, unsigned wlen, std::stringstream params){
	std::cout << "write-ack " << std::endl;
	
	std::string s1, s2, serr;
	unsigned addr;
	uint8_t data[wlen];
	
	params >> s1 >> s2;
	
	if( params >> serr ){
		std::cerr << "Unexpected extra parameter to write-req: " << serr;
		exit( EXIT_FAILURE );
	}
	
	addr	= std::stoi(s1, nullptr, 10);
	
	// assumes leading zeroes will be present if applicable
	for(unsigned i=0; i<wlen; ++i)
		data[i] = std::stoi( s2.substr(i*2, 2), nullptr, 16 );
	
#ifdef DEBUG
	std::cout << "Writing ";
	for(unsigned i=0; i<wlen; ++i)
		std::cout << std::hex << (int)data[i];
	std::cout << " to " << addr << std::endl;
#endif
	
	cache->write(addr, data);
	std::cout << " " << cache->set_idx() << " ";
	if( cache->hit() )
		std::cout << "hit ";
	else
		std::cout << "miss ";
	std::cout << cache->access_time() << std::endl;
}

void flush(Cache* cache){
	std::cout << "flush-ack" << std::endl;
}

void debug(Cache* cache){
	std::cout << "debug-ack-begin" << std::endl;
	std::cout << "debug-ack-end" << std::endl;
}
