#!/bin/sh

echo "#ifndef LIBOPENCRAFT_VERSION_H"  >include/libopencraft/version.h
echo "#define LIBOPENCRAFT_VERSION_H" >>include/libopencraft/version.h
echo "#define LIBOPENCRAFT_VERSION    \"0.1\"" >>include/libopencraft/version.h
echo "#define LIBOPENCRAFT_BUILD      \"`git show -s --pretty=format:%h`\"" >>include/libopencraft/version.h
echo "#define LIBOPENCRAFT_BUILDDATE  \"`date`\"" >>include/libopencraft/version.h
echo "#define LIBOPENCRAFT_SHORT_VER  \"libopencraft v\" LIBOPENCRAFT_VERSION" >>include/libopencraft/version.h
echo "#define LIBOPENCRAFT_LONG_VER   \"OpenCraft libopencraft version \" LIBOPENCRAFT_VERSION \" build \" LIBOPENCRAFT_BUILD " >>include/libopencraft/version.h
echo "#endif" >>include/libopencraft/version.h

