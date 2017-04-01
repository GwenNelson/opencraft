This directory contains some raw packet dumps used by the test suite.

Each packet is a simple uncompressed and unencrypted dump without the packet ID prefix. To actually use the packets, do the following:

 1. Use the bin2c tool (you'll find it in build/ after compiling libopencraft) to convert it into a C file
 2. In your code, calculate the size of the varint for the packet type and add this onto the size of the array
 3. Dump to the network the size, followed by the ID, followed by the array data

To make things simpler, you'll find all included packet dumps have been already converted. Some of these pre-generated headers contain a copy of the original fields so the test suite can do comparisions.
If you regenerate the header files you must include the relevant fields or the test suite will not run.
