#!/bin/sh

echo "#ifndef OPENCRAFT_VERSION_H"  >include/version.h
echo "#define OPENCRAFT_VERSION_H" >>include/version.h
echo "#define OPENCRAFT_VERSION    \"0.1\"" >>include/version.h
echo "#define OPENCRAFT_BUILD      \"`git show -s --pretty=format:%h`\"" >>include/version.h
echo "#define OPENCRAFT_BUILDDATE  \"`date`\"" >>include/version.h
echo "#define OPENCRAFT_SHORT_VER  \"opencraft-newview v\" OPENCRAFT_VERSION" >>include/version.h
echo "#define OPENCRAFT_LONG_VER   \"OpenCraft newview version \" OPENCRAFT_VERSION \" build \" OPENCRAFT_BUILD " >>include/version.h
echo "#endif" >>include/version.h

