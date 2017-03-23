#!/bin/sh

echo "#pragma once"  >include/opencraft/versiondefs.h
echo "#define OPENCRAFT_FRAMEWORK_VERSION    \"0.1\"" >>include/opencraft/versiondefs.h
echo "#define OPENCRAFT_FRAMEWORK_BUILD      \"`git show -s --pretty=format:%h`\"" >>include/opencraft/versiondefs.h
echo "#define OPENCRAFT_FRAMEWORK_BUILDDATE  \"`date`\"" >>include/opencraft/versiondefs.h
echo "#define OPENCRAFT_FRAMEWORK_SHORT_VER  \"OpenCraft v\" OPENCRAFT_FRAMEWORK_VERSION" >>include/opencraft/versiondefs.h
echo "#define OPENCRAFT_FRAMEWORK_LONG_VER   \"OpenCraft framework version \" OPENCRAFT_FRAMEWORK_VERSION \" build \" OPENCRAFT_FRAMEWORK_BUILD " >>include/opencraft/versiondefs.h
echo "#define OPENCRAFT_FRAMEWORK_COPYRIGHT  \"Copyright(C) 2017 Gareth Nelson, This is free software, see LICENSE file for details\"" >>include/opencraft/versiondefs.h

git show -s --pretty=format:%h >build/buildno.txt
echo "0.1"                     >build/version.txt
