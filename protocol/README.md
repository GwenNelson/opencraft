# OpenCraft - libopencraft

This library is a C++ implementation of the minecraft protocol, intended for use internally within OpenCraft but may be of use to other projects.

See DESIGN for a very stream of conciousness style overview of the design.

# Building and testing the library

To build the library do the following:

 1. cd build
 2. ./build.sh

By default this will compile the actual library (libopencraft.so) as well as an executable named test_opencraft. Simply run the file to execute a test suite.

For faster compiles during development, do this instead:

 1. cd build
 2. cmake ..
 3. make -j

A few examples are also available in examples/
