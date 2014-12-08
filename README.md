Architecture II 2014, Coursework 2
==================================

Goals of this coursework
------------------------

There are two goals for this coursework:

- Make sure you understand the working of a cache, including
  the specifics of read misses versus write misses, and the
  managment of valid and dirty flags.
  
- Develop skills in working with input and output streams
  for programs, and methods for checking that files match
  a particular format.

Specification
-------------

Write a program which models the behaviour of
an arbitrary single-level cache configuration,
including both a cache, and the memory behind
the cache. The program will receive the configuration
as command line arguments, a stream of input memory
transactions, and produce a stream of response transactions.

The program should start with a memory full of zeros, and
an empty cache. Input transactions should be processed until
end-of-file on stdin, which will result in the program
exiting.

### Parameters

Input parameters (in order of appearance on
the command line) are:

1. Address bits

2. Bytes/Word

3. Words/Block

4. Blocks/Set

5. Sets/Cache

6. Hit time (cycles/Access)

7. Memory read time in cycles/Block

8. Memory write time in cycles/Block

All parameters are positive integers, encoded
as decimal. Parameters 2-5 will be integer powers of 2
(e.g. 1, 2, 4, ...) Total memory space size is at most 8MB.

The cache should operate a LRU replacement policy,
and a write-back write policy.

### Input

The input stream on `stdin` will consist of three types of
transaction, with the following patterns:

1. Read request: "read-req" address

2. Write request: "write-req" address data

3. Flush request: "flush-req"

4. Debug request: "debug-req"

5. Comments: "#" followed by any characters.

A flush request ensures that memory is consistent with
current cache contents (recalling that the cache is a
write-back cache). The end of the input stream will
be indicated with end-of-file.

A debug request results in user-defined debug output,
and can be used (for example) to print the current state
of the cache.

Comments do not produce any response, and should be discarded.

The maximum length of any line in the input string is 1020
characters.

### Output

In response, your program will produce a stream of
responses on `stdout`, describing the result of the transaction:

1. Read response (a): "read-ack" data set-index "hit"|"miss" time
              
2. Read response (b): "read-ack" set-index "hit"|"miss" time data

3. Write response: "write-ack" set-index "hit"|"miss" time

4. Flush response: "flush-ack" time

5. Debug response: "debug-ack-begin"\n<lines>\n"debug-ack-end"

6. Comments: "#" followed by any characters.

(The two formats for `read-ack` are to reflect a historical
lack of preciseness in the spec. A program should choose
one output form to use, and it is up to consumers to
work out which one is in effect. So this is an example
of de-facto specification, where the existance of mutually
incompatible standards in the wild pushes complexity
onto the user.)

`set-index` is the index of the set in which the cache
places the address.

The `time` associated with each response is the total
time (in cycles) needed by the memory system to service each request.
For read and write requests, all cache processing is included
within the hit time, and the only extra time needed is the
time taken to perform any reads and/or writes to memory. Flushes
do not include the hit time, and only consume the time taken to
write dirty pages to memory. In each case, the time should be the
minimum possible, so solutions which by-pass the cache for every
read and write are not correct.

A debug response will contain zero or more lines, sandwiched
between a line containing "debug-ack-begin" and "debug-ack-end".
The response can be empty, i.e.:

    debug-ack-begin
    debug-ack-end

or you may wish to include logic that prints the current state
of the cache for debugging purposes. Either way, the command must
be supported.

Comments can be produced at any time (or not at all), and
will be discarded.

### Data representation and format

All addresses are byte addresses. Both addresses
and set indices are decimal integers

Data is a string of hex digits with upper-case letters, which
must be of the same width as a cache word (i.e. two hex digits
per byte). The ordering is from highest address to lowest. For
example, the 64-bit data string

    FEDCBA9876543210

contains 0xFE at byte offset 7, and 0x10 at byte offset
zero. Note that there is no notion of endian-ness here,
as we are not inside the CPU.

Submission
----------

Your submission should be a zip file containing:

- A C++ source file called `mem_sim.cpp` which
  implements the simulator.
  
- (Optional) Any ancilliary C++ sources called `mem_sim_*.cpp`
  which should be compiled into the program.

- A file called `readme.txt` which briefly (i.e. no more than
  a few lines) explains how you tested and debugged your
  simulator.

- Any supporting files used during testing, such as test input
  and output, and/or scripts used. These are not part of the
  assessed work, they are there to explain how you arrived
  at the solution (e.g. if external examiners are interested,
  or marks need to be justified).
  
The submission should be submitted via blackboard.

Assessment
----------

The marks weighting is broken down as follows:

- 20% Specification: Are the files called the right thing,
  do they compile?

- 20% Input/output: Does the simulator parse all valid
  input files without crashing, and is the output always
  in the valid format (regardless of whether it is correct).

- 60% Correctness: Does the simulator produce the right output?
  This will be assessed on a number of inputs, from simple
  direct mapped (including the example input) to more complicated.
  
As before, there is a formative deadline, where I try your code
on a sub-set of tests, and try to point out if it is not following
the spec in an obvious way, before the final hard deadline.

Formative deadline: Thursday 4th 23:59.

Hard deadline: Thursday 11th 23:59.

Example input
-------------

The files `direct-mapped.input` and `direct-mapped.output` give
example input and output files for a cache with:

- Address bits = 8
- Bytes/Word = 2
- Words/Block = 2
- Blocks/Set = 1
- Sets/Cache = 2
- Cycles/Hit = 1
- Cycles/ReadBlock = 2
- Cycles/WriteBlock = 2

To execute your program with this input, you would
do:

    cat direct-mapped.input | ./mem_sim 8 2 2 1 2 1 2 2

in a unix-style command line, or:

    type direct-mapped.input | mem_sim 8 2 2 1 2 1 2 2

in a windows command prompt. In both cases the output
should be printed to stdout, and could be captured to
the file `direct-mapped.got` by redirecting:

    cat direct-mapped.input | ./mem_sim 8 2 2 1 2 1 2 2 > direct-mapped.got
  
or:

    type direct-mapped.input | mem_sim 8 2 2 1 2 1 2 2 > direct-mapped.got
    
Note that the method you use to get input and output to
your program depends on the OS you choose. As long as
your program is C or C++, takes its parameters as
command line arguments, its input from stdin, and
sends output to stdout, then it will work on any
platform/OS (which is all the spec cares about).

Notes
=====

Some things to notice
---------------------

### Single word blocks

If there is only one word per block, then write misses
can be optimised to be slightly faster than write misses
on multi-word blocks.

### LRU management

If you associate each valid (i.e. non empty) block in the
cache with the read or write request it was last (most recently)
accessed in, you will find that each block must be associated with
a unique request. If you assign each read/write request an
increasing index, then the block with the smallest index is
the least recently used.

Alternatively, if you consider the blocks in a set as a
list, then another approach is to keep moving the block
being accessed to the front of the list, while maintaining
the relative order of all other blocks. So if the current
order is [B0,B3,B2,B1], then B0 was MRU, and B1 was LRU.
If block B2 is now accessed, the order changes to [B2,B0,B3,B1].
Note that you don't have to move the blocks themselves
around, you just need a list of block indices.

Anticipated questions
---------------------

### How do I know if my cache works?

Creating input by hand works quite well for small
sizes, e.g. using a spreadsheet to manually track
what is in each block. You don't need to test every
combination of input parameters, only to exercise
a few interesting combinations. For example direct-mapped,
4-way, and fully associative combined with a few
different word and block sizes should suffice.

While it's a bit boring, it shouldn't take more than
5 minutes to generate both input and output, assuming
you have a good understanding of cache operation.
If you don't, then that probably shows you need to
spend more time with the book (which is rather the point
of the exercise).

### How can I read from stdin?

You can read from stdin using `fgets`, `scanf` or `std::cin`.
Similarly, `printf` and `std::cout` will write to stdout.

I have specified a maximum input length size to make your life
easier if you want to work with fixed-length buffers using
`fgets`. If you use a 1024 byte input buffer, it will always be
large enough.

Try not to use use `gets`, as you will make the baby
Jesus cry:
http://stackoverflow.com/questions/3302255/c-scanf-vs-gets-vs-fgets
However, I won't mark you down or anything.

### We haven't done language processors yet, how can we parse stuff?

This is a very restricted form of parsing, as you know
there are only four possible input types, each one is
solely contained within one line, and the structure of
each line is precisely fixed. There are simple
ways of doing this form of parsing, in either
C style or C++ style.

### How can I parse the input lines in C++?

In C++ style, just use the `>>` operator to bring
in the different parts. First read a string, and
it will be one of `read-req`, `write-req` etc. That
then tells you whether you need to read an address
(i.e. an integer), and possibly also a string of
hex characters. So something like:

    while(1){
        std::string cmd;
        
        std::cin>>cmd;
        // Handle end of file here
         
         
        if(cmd=="read-req"){
            unsigned addr;
            std::cin>>addr;
      
           // Now handle a read at address "addr"
        }else if(cmd=="write-req"){
            ...
    }

### How can I parse the input lines in C?

You can mimic the C++ style, using `fscanf` to
read each part of the input, first using it to
read a string and comparing it with `read-req` etc.,
then conditionally reading more arguments.

However, a more C-like way is to use `fgets` to read the
next input line into a character array, then use
`sscanf` to extract the different parts. If you
imagine the person producing the requests using
printf strings, then you can use sscanf with the
same string to parse it. 

    while(1){
        char buffer[1024]={0};
        
        fgets(buffer, sizeof(buffer), stdin);
        // Handle end of file
        
        unsigned address;
        if(sscanf(buffer, "read-req %u", &address)){
           // Handle a read request at "address" 
        }else{
           ... // other input patterns
        }
  
### What should I do if the input file is malformed?

For this exercise you don't need to worry about invalid
input. Ideally you would fail gracefully with an error
message highlighting the problem, but it is entirely
acceptable to crash, start producing gibberish, whatever.
The only thing you are not allowed to do is anything
deliberately malicious.
  
### How can I compare the output of my program against a reference?

There's a unix tool for that called `diff`, which will print
differences between two files.

### How can I ignore the differences in coments between output files?

There are a few tools which can do this, ranging from one liners
in scripting languages like python and perl, to tool such as awk
and sed: http://stackoverflow.com/a/5413132

One example is the sed command:

    sed -r -e "/#.*/d" -
  
Which will take input on stdin, and write it on stdout without
any comments.

### So I know there is a difference in the output, but how to debug it?

A useful thing to do is to line up the input, your reference output,
and the actual output from your program. Unsurprisingly, there
are many ways of doing that. One tool which will do it is `pr`,
so if you have (un-commented) files `test.input`, `test.output`, and
`test.got`, you could do:

    pr -m -t test.input test.output test.got

to get them printed out side by side.

### There are so many commands, why do I have to keep typing them in?

Well, you could put your testing commands in a script, which
runs your program, does any comment stripping, prints the
columns side by side, and also diffs the output. Something
like:

    #!/bin/bash
    INPUT=test.input;
    OUTPUT=test.output;
    GOT=test.got;
    
    # Run your program, write the output to file name in GOT
    cat ${INPUT}  |  ./mem_sim 8 2 2 1 2 1 2 2  >  ${GOT}
    
    # Look at the differences (this assumes no comments)
    diff ${OUTPUT} ${GOT}

You can stick pretty much any commands in a script, and
then execute them in one go (including compilation of
the program if you want). The main steps are:

1. Create a text file (e.g. `shell.sh`)

2. Make sure the text file starts with `#!/bin/bash` (or
   the name of some other shell), then put other comands
   on a following line.
   
3. Make sure the text file is marked as executable, using
   `chmod u+x shell.sh`.

4. Run it, remembering to use a leading `./` if you are in
  the same directory (e.g. `./script.sh`).

### What if I don't want to use shell scripts?

Well, don't. Just type it in.

### What if I'm using OSX, so those tools aren't available?

Hopefully no-one on EIE would actually ask that.

### What if I'm using Windows, so those tools aren't available?

Well, you could install cygwin (or mingw if you are
more hard-core), and all the tools will then be available.

Alternatively you could implement all of it yourself,
writing a program that strips comments, prints them
side by side, and highlights differences. It requires
about 10 lines of C.

### What does shell script have to do with Computer Architecture?

Nothing. You can manually run all the commands each
time, or build a GUI in Visual Basic to do testing, or
not do any testing at all, or whatever you feel like.

However, scripting is related to general computing,
and these marks go into your Computer Lab module
marks, so the idea is that you develop general
computing skills while doing an exercise which
develops your understanding of architecture. Working
with files in a defined format is explicitly part of
the exercise, doing so efficiently is only an implicit
part.

### Ugh, this is another massively complicated exercise that will take forever.

Not really a question, but I'll answer it.

While it may look complicated (or it may not), this is
actually much simpler than the MIPS exercise. There
are a few fairly short steps:

1. Create a simple parser which can read the example
  input, split it into the arguments, and print it to
  the output. This takes about 10 lines of code.

2. Develop test inputs/outputs (do this second, to ensure
   they are in the right format). This should also check you
   know exactly how the cache should operate.

3. Define the cache and memory data-structures. This just
  follows the heirarchy of hardware in a cache, and takes
  very little time.

4. Map the input commands from your parser to operations on
   the cache.

5. Test and debug.

Of these steps, the most complicated are arguably 2, making
sure you understand how a write-back LRU cache works, and 5,
which is making sure your code works.
