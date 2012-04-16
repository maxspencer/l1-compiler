L1 Compiler
===========

A compiler for the language L1 which is a very simple example language taught as part of Part 1b of the Computer Science Tripos at Cambridge University, UK. For more information about the language see [the course notes](http://www.cl.cam.ac.uk/teaching/1112/Semantics/notes.pdf), pages 28-9.

The primary objective of the project was for me to learn about compilers and programming in C, not to write a useful compiler, or even particularly sensible code. In some places I have done things in rather odd ways just to see how stuff works. Though in some places, I expect I have done things in stupid ways unintentionally as well..

Prerequisites
-------------

You'll need flex and byacc installed. Should be able to get them with `sudo apt-get install flex byacc`.

Compilation and Usage
---------------------

I have only compiled and run the code on my own machine which has Ubuntu 10.04 and I haven't specifically tried to make it widely compatible. I'm sorry if this doesn't work for you.

`make l1c` to get the l1c binary.

then `./l1c [options] <target file>` to compile something! I plan to add a `--help` flag in the next commit to explain the options. Also one of the "options" to output jvm bytecode is actually not optional so I need to fix that.

For some immediate gratification, just do `./l1c -d tree.dot test.l1` and then check out the tree.dot and bytecode.jvm files that are generated.

Closing Comments
----------------

Criticism and comments of any kind will be greatly appreciated. I have actually spent a lot more time that I originally intended to doing this so it would be nice to get some feedback. I want to add a code generation phase to get from JVM bytecode to MIPS or something, I don't think that will be particularly difficult and it will be a good opportunity to do some peephole optimisations.

Many thanks,

Max Spencer