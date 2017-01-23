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
#include <handshake.packet.h>

#include <arpa/inet.h>

#include <string>
#include <iostream>
#include <exception>
#include <vector>

#include <proto_constants.h>

using namespace std;

std::string failmsg = "";

float tests_run;
float tests_passed;
float tests_failed;

typedef bool (*testcase_t)();

void run_test(std::string desc, testcase_t test) {
     failmsg = "";
     cout << "Testing " << desc;
     cout.flush();
     tests_run+=1.0f;
     bool retval=false;
     try {
       retval = test();
     } catch(exception& e) {
       tests_failed+=1.0f;
       cout << ": FAIL - Exception: " << e.what() << endl;
       return;
     }
     if(retval) {
       tests_passed+=1.0f;
       cout << ": PASS" << endl;
     } else {
       cout << ": FAIL: " << failmsg << endl;
       tests_failed+=1.0f;
     }
}

bool create_handshake() {
     opencraft::packets::handshake_handshaking_upstream newpack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);
     return true; // if an exception occurred above, we failed - otherwise always succeed
}

bool check_handshake_name() {
     opencraft::packets::handshake_handshaking_upstream newpack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);
     if(newpack.name()=="handshake") return true;
     failmsg = "name did not match, got " + newpack.name() + " instead of handshake";
     return false;
}

bool serialise_handshake() {
     opencraft::packets::handshake_handshaking_upstream newpack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);     
     std::vector<unsigned char> packdata = newpack.pack();
     if(packdata.size() != 0) return true;
     failmsg = "pack() method returned 0-length vector";
     return false;
}

bool unserialise_handshake() {
     opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);     
     std::vector<unsigned char> packdata =hspack.pack();
     opencraft::packets::opencraft_packet *newpack = opencraft::packets::opencraft_packet::unpack_packet(OPENCRAFT_STATE_HANDSHAKING,packdata);

     bool retval=false;
     failmsg = "Got NULL from unpack_packet";
     if(newpack != NULL) retval=true;
     delete newpack;
     return retval;
}

bool unserialise_handshake_samevalues() {
     opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);     
     std::vector<unsigned char> packdata =hspack.pack();
     opencraft::packets::handshake_handshaking_upstream *newpack = (opencraft::packets::handshake_handshaking_upstream*)opencraft::packets::opencraft_packet::unpack_packet(OPENCRAFT_STATE_HANDSHAKING,packdata);
     
     if(newpack == NULL) {
        failmsg = "Got NULL from opencraft_packet::unpack_packet";
        return false;
     }

     bool retval = true;
     if(newpack->a != OPENCRAFT_PROTOCOL_VERSION) {
        failmsg = "\nExpected a=" + to_string(OPENCRAFT_PROTOCOL_VERSION) + ", got a=" + to_string(newpack->a);
        retval = false;
     }
     if(newpack->b != std::string(OPENCRAFT_DEFAULT_SERVER)) {
        failmsg += "\nExpected b=\"" + std::string(OPENCRAFT_DEFAULT_SERVER) + "\", got b=\"" + newpack->b + "\"";
        retval = false;
     }
     if(newpack->c != OPENCRAFT_DEFAULT_TCP_PORT) {
        failmsg += "\nExpected c=" + to_string(OPENCRAFT_DEFAULT_TCP_PORT) + ", got c=" + to_string(newpack->c);
        retval = false;
     }
     if(newpack->d != OPENCRAFT_STATE_STATUS) {
        failmsg += "\nExpected d=" + to_string(OPENCRAFT_STATE_STATUS) +", got d=" + to_string(newpack->d);
        retval = false;
     }
     delete newpack;
     return retval;
}


bool unpack_file_via_constructor() {
     std::vector<unsigned char> packdata;
     for(int i=0; i<handshake_packet_length; i++) {
         packdata.push_back((unsigned char)handshake_packet[i]);
     }
     opencraft::packets::handshake_handshaking_upstream hspack(packdata);
     return true;
}

bool unpack_handshakefile_fielda() {
     std::vector<unsigned char> packdata;
     for(int i=0; i<handshake_packet_length; i++) {
         packdata.push_back((unsigned char)handshake_packet[i]);
     }
     opencraft::packets::handshake_handshaking_upstream hspack(packdata);
     if(hspack.a == handshake_packet_a) return true;
     return false;
}

bool unpack_handshakefile_fieldb() {
     std::vector<unsigned char> packdata;
     for(int i=0; i<handshake_packet_length; i++) {
         packdata.push_back((unsigned char)handshake_packet[i]);
     }
     opencraft::packets::handshake_handshaking_upstream hspack(packdata);
     if(hspack.b == handshake_packet_b) return true;
     return false;
}

bool unpack_handshakefile_fieldc() {
     std::vector<unsigned char> packdata;
     for(int i=0; i<handshake_packet_length; i++) {
         packdata.push_back((unsigned char)handshake_packet[i]);
     }
     opencraft::packets::handshake_handshaking_upstream hspack(packdata);
     if(hspack.c == handshake_packet_c) return true;
     return false;
}

bool unpack_handshakefile_fieldd() {
     std::vector<unsigned char> packdata;
     for(int i=0; i<handshake_packet_length; i++) {
         packdata.push_back((unsigned char)handshake_packet[i]);
     }
     opencraft::packets::handshake_handshaking_upstream hspack(packdata);
     if(hspack.d == handshake_packet_d) return true;
     return false;
}

bool packed_127_equal_unpacked() {
     int32_t     testval=127;
     unsigned char packedval[4];
     opencraft::packets::unparse_varint(testval,packedval);
     int32_t unpacked_val = opencraft::packets::parse_var_int(packedval,4);
     if(unpacked_val==testval) return true;     
     return false;
}

bool packed_128_equal_unpacked() {
     int32_t     testval=128;
     unsigned char packedval[4];
     opencraft::packets::unparse_varint(testval,packedval);
     int32_t unpacked_val = opencraft::packets::parse_var_int(packedval,4);
     if(unpacked_val==testval) return true;     
     std::cerr << "\nGot " << unpacked_val << " isntead of 128\n";
     return false;
}

bool packed_130_equal_unpacked() {
     int32_t     testval=130;
     unsigned char packedval[4];
     opencraft::packets::unparse_varint(testval,packedval);
     int32_t unpacked_val = opencraft::packets::parse_var_int(packedval,4);
     if(unpacked_val==testval) return true;     
     std::cerr << "\nGot " << unpacked_val << " isntead of 130\n";
     return false;
}



int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;

    run_test("Create handshake packet",&create_handshake);
    run_test("Check handshake packet has valid name",&check_handshake_name);
    run_test("Serialise handshake packet returns vector of non-zero length",&serialise_handshake);
    run_test("Unserialising handshake packet returns none-NULL",&unserialise_handshake);
    run_test("Unserialised handshake packet has same values as original packet",&unserialise_handshake_samevalues);
    run_test("Unpack handshake.packet file via handshake packet class constructor does not crash",&unpack_file_via_constructor);
    run_test("Unpacked handshake.packet file has correct field A",&unpack_handshakefile_fielda);
    run_test("Unpacked handshake.packet file has correct field B",&unpack_handshakefile_fieldb);
    run_test("Unpacked handshake.packet file has correct field C",&unpack_handshakefile_fieldc);
    run_test("Unpacked handshake.packet file has correct field D",&unpack_handshakefile_fieldd);
    run_test("127 Packed as varint has same value when unpacked",&packed_127_equal_unpacked);
    run_test("128 Packed as varint has same value when unpacked",&packed_128_equal_unpacked);
    run_test("130 Packed as varint has same value when unpacked",&packed_130_equal_unpacked);


    cout << endl;
    cout << tests_passed << "/" << tests_run << " Passed" << endl;
    cout << tests_failed << "/" << tests_run << " Failed" << endl;
    cout << "Pass rate: " << (tests_passed/tests_run)*100.0f << endl;
}




