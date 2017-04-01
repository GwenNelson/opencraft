#!/bin/bash
pushd common
./genversion.sh
cd build
cmake ..
make
popd

pushd protocol/build
cmake ..
make opencraft-protocol
make
popd

mkdir -p dist/bin
mkdir -p dist/include
mkdir -p dist/lib

cp -v  common/build/libopencraft-common.so dist/lib/
cp -v  protocol/build/libopencraft-protocol.so  dist/lib/

cp -Rv common/include/* dist/include/
cp -Rv protocol/include/* dist/include/

