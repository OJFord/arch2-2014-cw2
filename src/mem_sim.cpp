//
//  mem_sim.cpp
//  LRU-CacheSim
//
//  Created by Ollie Ford on 25/11/2014.
//  Copyright (c) 2014 OJFord. All rights reserved.
//

#include "mem_sim.h"

int main(int argc, const char * argv[]){
	int addr, bpw, wpb, bps, spc, time_hit, time_write, time_read;
	
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

	std::string input;
	while( std::getline(std::cin, input) ){
		
		if( input.compare(0, 1, "#") == 0 );
			//Comment. Ignore until newline.
		
		else if( input.compare(0, 9, "read-req ") == 0 )
			read( std::stringstream(input.substr(9)) );
		
		else if( input.compare(0, 10, "write-req ") == 0)
			write( std::stringstream(input.substr(9)), bpw);
		
		else if( input.compare(0, 10, "flush-req ") == 0)
			flush();
		
		else if( input.compare(0, 10, "debug-req ") == 0)
			debug();
		
		else
			std::cerr << "Malformed input." << std::endl;
		
	}
	
	exit( EXIT_SUCCESS );
}

void read( std::stringstream params ){
	std::cout << "read-ack" << std::endl;
	
	std::string s, serr;
	params >> s;
	
	if( params >> serr )
		std::cerr << "Unexpected extra parameter to write-req: " << serr;

	int addr = std::stoi(s);
	
	std::cout << "Reading " << addr << std::endl;
}

void write( std::stringstream params, int bytesPerWord){
	std::cout << "write-ack" << std::endl;
	
	std::string s1, s2, serr;
	int addr;
	uint8_t data[ bytesPerWord ];
	
	params >> s1 >> s2;
	
	if( params >> serr )
		std::cerr << "Unexpected extra parameter to write-req: " << serr;
	
	addr	= std::stoi(s1, nullptr, 10);
	for(unsigned i=0; i<bytesPerWord; ++i)
		data[i] = std::stoi( s2.substr(i*2, 2), nullptr, 16 );
	
	std::cout << s2 << std::endl;
	
	std::cout << "Writing ";
	for(unsigned i=0; i<bytesPerWord; ++i)
		std::cout << std::hex << (int)data[i];
	std::cout << " to " << addr << std::endl;
}

void flush(void){
	std::cout << "flush-ack" << std::endl;
}

void debug(void){
	std::cout << "debug-ack-begin" << std::endl << std::endl << "debug-ack-end" << std::endl;
}
