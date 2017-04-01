#!/bin/sh

echo "#ifndef OPENCRAFT_LAUNCHER_VERSION_H"  >include/version.h
echo "#define OPENCRAFT_LAUNCHER_VERSION_H" >>include/version.h
echo "#define OPENCRAFT_LAUNCHER_VERSION    \"0.1\"" >>include/version.h
echo "#define OPENCRAFT_LAUNCHER_BUILD      \"`git show -s --pretty=format:%h`\"" >>include/version.h
echo "#define OPENCRAFT_LAUNCHER_BUILDDATE  \"`date`\"" >>include/version.h
echo "#define OPENCRAFT_LAUNCHER_SHORT_VER  \"opencraft-launcher v\" OPENCRAFT_LAUNCHER_VERSION" >>include/version.h
echo "#define OPENCRAFT_LAUNCHER_LONG_VER   \"OpenCraft launcher version \" OPENCRAFT_LAUNCHER_VERSION \" build \" OPENCRAFT_LAUNCHER_BUILD " >>include/version.h
echo "#endif" >>include/version.h

