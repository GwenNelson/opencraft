//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
//
// The OpenCraft client is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft client is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//     Test suite
//
//-----------------------------------------------------------------------------


#include <packets.autogen.h>
#include <version.h>
#include <string>
#include <iostream>
#include <exception>

using namespace std;

unsigned int tests_run;
unsigned int tests_passed;
unsigned int tests_failed;

typedef bool (*testcase_t)();

void run_test(std::string desc, testcase_t test) {
     cout << "Testing " << desc;
     tests_run++;
     bool retval=false;
     try {
       retval = test();
     } catch(exception& e) {
       tests_failed++;
       cout << ": FAIL - Exception: " << e.what() << endl;
       return;
     }
     if(retval) {
       tests_passed++;
       cout << ": PASS" << endl;
     } else {
       cout << ": FAIL" << endl;
       tests_failed++;
     }
}

bool always_fail() {
     return false;
}

int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;

    run_test("Always fail",&always_fail);

    cout << endl;
    cout << tests_passed << "/" << tests_run << " Passed" << endl;
    cout << tests_failed << "/" << tests_run << " Failed" << endl;
}




