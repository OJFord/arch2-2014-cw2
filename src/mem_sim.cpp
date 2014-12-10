//
//  mem_sim.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 25/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#include "mem_sim.h"
#include "mem_sim_lrque.h"
#include <fstream>

int main(int argc, const char * argv[]){
	
#ifdef DEBUG		// pipe input file to stdin
	std::ifstream arq(getenv("SAMPLE_INPUT"));
	std::cin.rdbuf(arq.rdbuf());
#endif
	
	unsigned alen, bpw, wpb, bps, spc, time_hit, time_write, time_read;
	
	if( argc < 8 ){
		std::cerr << "Insufficient parameters supplied." << std::endl;
		exit( EXIT_FAILURE );
	}
	
	alen= atoi( argv[1] );
	bpw	= atoi( argv[2] );
	wpb	= atoi( argv[3] );
	bps	= atoi( argv[4] );
	spc	= atoi( argv[5] );
	
	time_hit	= atoi( argv[6] );
	time_read	= atoi( argv[7] );
	time_write	= atoi( argv[8] );

	Ram*	ram	= new Ram(alen, bpw);
	
	// LRU block replacement policy
	Cache<lrque>* cache = new Cache<lrque>(ram, alen, spc, bps, wpb, bpw,
										   time_hit, time_read, time_write);

	std::string input;
	while( std::getline(std::cin, input) ){
		
		if( input.compare(0, 1, "#") == 0 );
			//Comment. Ignore until newline.
		
		else if( input.compare(0, 9, "read-req ") == 0 )
			read( cache, bpw, std::stringstream(input.substr(9)) );
		
		else if( input.compare(0, 10, "write-req ") == 0)
			write( cache, bpw, std::stringstream(input.substr(9)) );
		
		else if( input == "flush-req" )
			flush( cache );
		
		else if( input == "debug-req" )
			debug( cache );
		
		else
			std::cerr << "Malformed input." << std::endl;
		
	}
	
	exit( EXIT_SUCCESS );
}
