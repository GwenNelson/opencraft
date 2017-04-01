#!/bin/sh

echo "#ifndef OPENCRAFT_NEWVIEW_VERSION_H"  >include/version.h
echo "#define OPENCRAFT_NEWVIEW_VERSION_H" >>include/version.h
echo "#define OPENCRAFT_NEWVIEW_VERSION    \"0.1\"" >>include/version.h
echo "#define OPENCRAFT_NEWVIEW_BUILD      \"`git show -s --pretty=format:%h`\"" >>include/version.h
echo "#define OPENCRAFT_NEWVIEW_BUILDDATE  \"`date`\"" >>include/version.h
echo "#define OPENCRAFT_NEWVIEW_SHORT_VER  \"opencraft-newview v\" OPENCRAFT_NEWVIEW_VERSION" >>include/version.h
echo "#define OPENCRAFT_NEWVIEW_LONG_VER   \"OpenCraft newview version \" OPENCRAFT_NEWVIEW_VERSION \" build \" OPENCRAFT_NEWVIEW_BUILD " >>include/version.h
echo "#endif" >>include/version.h

