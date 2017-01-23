# OpenCraft - libopencraft

This library is a C++ implementation of the minecraft protocol, intended for use internally within OpenCraft but may be of use to other projects.

See DESIGN for a very stream of conciousness style overview of the design.

# Building and testing the library

To build the library do the following:

 1. mkdir -p build
 2. cd build
 3. cmake ..
 4. make

By default this will compile the actual library (libopencraft.so) as well as an executable named test_opencraft. Simply run the file to execute a test suite.
