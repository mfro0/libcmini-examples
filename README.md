# libcmini-examples
a few simple example programs to show how to deal with the libcmini library
At least, that was the initial intent. Meanwhile, some of the programs (bench, in particular) evolved into a testbed for many GEM related things I'm playing with.
Don't expect the code to build and run at all times. If it doesn't I'm probably in the middle of some experiments. You can then try checking out one of the earlier commits that might work.

just clone this to the same directory level than your libcmini dir and make

The CPX examples allow to create CPXs with the gcc toolchain. gcc calling conventions normally don't allow calling functions that aren't compiled by gcc itself and have short arguments through function pointers as gcc always pushes short arguments longword-aligned. That makes gcc unsuitable to call the functions provided by the CPX application directly.
The trick is to pack the argument list into a struct and use that instead as a single argument. gcc then pushes the entire packed struct which perfectly mimics the calling convention of other Atari C compilers. The CPX startup code provided implements an intermediate layer that hides this quirk, so inside the CPX there is no need to care about this.
colorvdi.cpx is converted from a source I found on the Internet. Although this doesn't have any copyright notice, it entirely possible the sources originate from Atari itself. 

make all
