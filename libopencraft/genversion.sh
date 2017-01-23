#!/bin/sh

echo "#ifndef LIBOPENCRAFT_VERSION_H"  >include/version.h
echo "#define LIBOPENCRAFT_VERSION_H" >>include/version.h
echo "#define LIBOPENCRAFT_VERSION    \"0.1\"" >>include/version.h
echo "#define LIBOPENCRAFT_BUILD      \"`git show -s --pretty=format:%h`\"" >>include/version.h
echo "#define LIBOPENCRAFT_BUILDDATE  \"`date`\"" >>include/version.h
echo "#define LIBOPENCRAFT_SHORT_VER  \"libopencraft v\" LIBOPENCRAFT_VERSION" >>include/version.h
echo "#define LIBOPENCRAFT_LONG_VER   \"OpenCraft libopencraft version \" LIBOPENCRAFT_VERSION \" build \" LIBOPENCRAFT_BUILD " >>include/version.h
echo "#endif" >>include/version.h

