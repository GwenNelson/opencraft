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
#include <proto_constants.h>
#include <string>
#include <iostream>
#include <exception>
#include <vector>

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

bool create_handshake() {
     opencraft::packets::handshake_handshaking_upstream newpack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);
     return true; // if an exception occurred above, we failed - otherwise always succeed
}

bool check_handshake_name() {
     opencraft::packets::handshake_handshaking_upstream newpack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);
     if(newpack.name()=="handshake") return true;
     return false;
}

bool serialise_handshake() {
     opencraft::packets::handshake_handshaking_upstream newpack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);     
     std::vector<unsigned char> packdata = newpack.pack();
     if(packdata.size() != 0) return true;
     return false;
}

bool unserialise_handshake() {
     opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);     
     std::vector<unsigned char> packdata =hspack.pack();
     opencraft::packets::opencraft_packet *newpack = opencraft::packets::opencraft_packet::unpack_packet(packdata);

     bool retval=false;
     if(newpack != NULL) retval=true;
     delete newpack;
     return retval;
}


int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;

    run_test("Create handshake packet",&create_handshake);
    run_test("Check handshake packet has valid name",&check_handshake_name);
    run_test("Serialise handshake packet returns vector of non-zero length",&serialise_handshake);
    run_test("Unserialising handshake packet returns none-NULL",&unserialise_handshake);
    
    cout << endl;
    cout << tests_passed << "/" << tests_run << " Passed" << endl;
    cout << tests_failed << "/" << tests_run << " Failed" << endl;
}




