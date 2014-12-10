Compiling with:

	g++ src/*.cpp -o memsim -std=c++11

Under:
	Apple LLVM version 6.0 (clang-600.0.56) (based on LLVM 3.5svn)

Testing with:

	diff					\
	 <(cat direct-mapped.input		\
		| ./memsim 8 2 2 1 2 1 2 2	\
		| sed -e “/#.*/d;”)		\
	 <(cat direct-mapped.output		\
		| sed -e “/#.*/d;”)

And with Xcode’s debugger when things really break.