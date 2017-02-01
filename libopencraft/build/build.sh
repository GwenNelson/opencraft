#!/bin/bash
pushd ..
./autogen.sh
popd
cmake ..
make -j
