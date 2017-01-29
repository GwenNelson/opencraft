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


#include <libopencraft/packets.autogen.h>
#include <libopencraft/version.h>
#include <libopencraft/proto_constants.h>
#include <handshake.packet.h>
#include <mc_login.bin.h>
#include <libopencraft/raw_packet.h>
#include <libopencraft/packet_stream.h>

#include <arpa/inet.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <exception>
#include <vector>

#include <libopencraft/proto_constants.h>

using namespace std;

std::string failmsg = "";

float tests_run;
float tests_passed;
float tests_failed;

typedef bool (*testcase_t)();

std::string dump_hex_vector(std::vector<unsigned char> v) {
     std::stringstream retstream;
     for(int i=0; i<v.size(); i++) {
         retstream << std::setfill('0') << std::setw(2) << std::hex << (uint32_t)v[i];
     }
     return retstream.str();
}

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

bool create_raw() {
     std::vector<unsigned char> test_vector;
     test_vector.push_back((unsigned char)1);
     test_vector.push_back((unsigned char)2);

     int32_t ident = 666;
     int32_t size  = test_vector.size();

     opencraft::packets::raw_packet raw_pack_a(size,ident,test_vector);
     std::vector<unsigned char> packdata = raw_pack_a.pack();

     opencraft::packets::raw_packet raw_pack_b(packdata);
     
     bool retval=true;
     if(raw_pack_b.pack_ident != ident) {
        failmsg += "\nexpected ident=666, got ident=" + to_string(raw_pack_b.pack_ident);
        retval = false;
     }
     if(raw_pack_b.pack_length != raw_pack_a.pack_length) {
        failmsg += "\nexpected length=" + to_string(raw_pack_a.pack_length) + ", got length=" + to_string(raw_pack_b.pack_length);
        retval = false;
     }
     if(raw_pack_b.pack_data.size() != test_vector.size()) {
        failmsg += "\npack_data does not match test_vector";
        failmsg += "\npack_data is size " + to_string(raw_pack_b.pack_data.size());
        failmsg += "\ntest_vector is size " + to_string(test_vector.size());
        failmsg += "\npack_data contains    :" + dump_hex_vector(raw_pack_b.pack_data);
        failmsg += "\ntest_vector contains  :" + dump_hex_vector(test_vector);
        retval = false;
     }
     if(raw_pack_b.pack_data[0] != 1) {
        failmsg += "\ntest_vector[0] should be 1, instead got " + to_string(raw_pack_b.pack_data[0]);
        retval = false;
     }
     return retval;
}

bool create_raw_ident() {
     std::vector<unsigned char> test_vector;
     test_vector.push_back((unsigned char)1);
     test_vector.push_back((unsigned char)2);

     int32_t ident=666;
     opencraft::packets::raw_packet raw_pack_a(ident,test_vector);

     opencraft::packets::raw_packet raw_pack_b(raw_pack_a.pack());

     if(raw_pack_b.pack_ident != ident) {
        failmsg += "\nexpected ident=666, got ident=" + to_string(raw_pack_b.pack_ident);
        return false;
     }
     return true;
}

bool check_handshake_name() {
     opencraft::packets::handshake_handshaking_upstream newpack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);
     if(newpack.name()=="handshake") return true;
     failmsg = "\nname did not match, got " + newpack.name() + " instead of handshake";
     return false;
}

bool serialise_handshake() {
     opencraft::packets::handshake_handshaking_upstream newpack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);     
     std::vector<unsigned char> packdata = newpack.pack();
     if(packdata.size() != 0) return true;
     failmsg = "\npack() method returned 0-length vector";
     return false;
}

bool unserialise_handshake() {
     opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);     

     opencraft::packets::raw_packet raw_pack(hspack.ident(),hspack.pack());
     opencraft::packets::opencraft_packet *newpack = opencraft::packets::opencraft_packet::unpack_packet(OPENCRAFT_STATE_HANDSHAKING,false,raw_pack.pack());

     bool retval=false;
     failmsg += "\nGot NULL from unpack_packet";
     failmsg += "\nhspack:                 " + dump_hex_vector(hspack.pack());
     failmsg += "\n  hspack length:        " + to_string(hspack.packed.size());
     failmsg += "\n  hspack ident:         " + to_string(hspack.ident());
     failmsg += "\nraw_pack.pack_data:     " + dump_hex_vector(raw_pack.pack_data);
     failmsg += "\nraw_pack:               " + dump_hex_vector(raw_pack.pack());
     failmsg += "\n  raw_pack.pack_length: " + to_string(raw_pack.pack_length);
     failmsg += "\n  raw_pack.pack_ident:  " + to_string(raw_pack.pack_ident);
     if(newpack != NULL) retval=true;
     delete newpack;
     return retval;
}

bool unserialise_handshake_via_constructor() {
     opencraft::packets::handshake_handshaking_upstream hspack_a(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);
     std::vector<unsigned char> packdata = hspack_a.pack();
     
     opencraft::packets::handshake_handshaking_upstream hspack_b(packdata);
     bool retval=true;
     if(hspack_b.a != hspack_a.a) {
        failmsg = "\nExpected a=" + to_string(hspack_a.a) + ", got a=" + to_string(hspack_b.a);
        retval = false;
     }
     if(hspack_b.b != hspack_a.b) {
        failmsg += "\nExpected b=\"" + std::string(hspack_a.b) + "\", got b=\"" + hspack_b.b + "\"";
        retval = false;
     }
     if(hspack_b.c != hspack_a.c) {
        failmsg += "\nExpected c=" + to_string(hspack_a.c) + ", got c=" + to_string(hspack_b.c);
        retval = false;
     }
     if(hspack_b.d != hspack_a.d) {
        failmsg += "\nExpected d=" + to_string(hspack_a.d) +", got d=" + to_string(hspack_b.d);
        retval = false;
     }
     if(!retval) {
        failmsg += "\nOriginal packet data: " + dump_hex_vector(hspack_a.packed);
        failmsg += "   a=" + to_string(hspack_a.a) + ", b=\"" + std::string(hspack_a.b) + "\", c=" + to_string(hspack_a.c) + ", d=" + to_string(hspack_a.d);
        failmsg += "\nReceived packet data: " + dump_hex_vector(hspack_b.packed);
        failmsg += "   a=" + to_string(hspack_b.a) + ", b=\"" + std::string(hspack_b.b) + "\", c=" + to_string(hspack_b.c) + ", d=" + to_string(hspack_b.d);
     }

     return retval;
}

bool unserialise_handshake_viaunpack_samevalues() {
     opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string(OPENCRAFT_DEFAULT_SERVER),OPENCRAFT_DEFAULT_TCP_PORT,OPENCRAFT_STATE_STATUS);     
     std::vector<unsigned char> packdata =hspack.pack();

     opencraft::packets::raw_packet raw_pack(hspack.ident(),packdata);
     opencraft::packets::handshake_handshaking_upstream *newpack = (opencraft::packets::handshake_handshaking_upstream*)opencraft::packets::opencraft_packet::unpack_packet(OPENCRAFT_STATE_HANDSHAKING,false,raw_pack.pack());
     
     if(newpack == NULL) {
        failmsg = "\nGot NULL from opencraft_packet::unpack_packet";
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
     if(!retval) {
        failmsg += "\nOriginal packet data: " + dump_hex_vector(hspack.packed);
        failmsg += "\nReceived packet data: " + dump_hex_vector(newpack->packed);
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

bool deserialise_captured() {
     std::vector<unsigned char> raw_data;
     for(int i=0; i< mclogin_bin_length; i++) {
         raw_data.push_back(mclogin_bin[i]);
     }
     opencraft::packets::opencraft_packet *newpack = opencraft::packets::opencraft_packet::unpack_packet(OPENCRAFT_STATE_HANDSHAKING,false,raw_data);
     opencraft::packets::handshake_handshaking_upstream *hspack = (opencraft::packets::handshake_handshaking_upstream*)newpack;
     bool retval = true;
     if(hspack->a != mclogin_bin_a) {
        failmsg += "\na != " + to_string(mclogin_bin_a);
        failmsg += "\nExpected " + to_string(mclogin_bin_a) + ", got " + to_string(hspack->a);
        retval   = false;
     }
     if(hspack->b != mclogin_bin_b) {
        failmsg += "\nb != \"" + mclogin_bin_b + "\"";
        failmsg += "\nExpected \"" + mclogin_bin_b + "\", got \"" + hspack->b + "\"";
        retval = false;
     }
     if(hspack->c != mclogin_bin_c) {
        failmsg += "\nc != " + to_string(mclogin_bin_c);
        failmsg += "\nExpected " + to_string(mclogin_bin_c) + ", got " + to_string(hspack->c);
        retval   = false;
     }
     if(hspack->d != mclogin_bin_d) {
        failmsg += "\nd != " + to_string(mclogin_bin_d);
        failmsg += "\nExpected " + to_string(mclogin_bin_d) + ", got " + to_string(hspack->d);
        retval   = false;
     }
     delete newpack;
     return retval;
 
}

bool simple_packstream() {
     opencraft::packets::packet_stream pack_stream;

     opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string("127.0.0.1"),25565,OPENCRAFT_STATE_STATUS);
     opencraft::packets::raw_packet raw_hs(hspack.ident(),hspack.pack());

     vector<opencraft::packets::raw_packet> inpacks = pack_stream.on_recv(raw_hs.pack());

     if(inpacks.size() != 1) {
        failmsg += "pack_stream.on_recv() expected return vector of length 1, got vector of length " + to_string(inpacks.size());
        return false;
     }

     if(inpacks[0].pack_ident != hspack.ident()) {
        failmsg += "packet ident: expected " + to_string(hspack.ident()) + ", got " + to_string(inpacks[0].ident());
        return false;
     }
     opencraft::packets::handshake_handshaking_upstream *newpack = (opencraft::packets::handshake_handshaking_upstream*)opencraft::packets::opencraft_packet::unpack_packet(OPENCRAFT_STATE_HANDSHAKING,false,inpacks[0].pack());
     
     if(newpack==NULL) {
        failmsg += "unpack returned NULL";
        return false;
     }

     bool retval = true;
     if(newpack->a != hspack.a) {
       failmsg += "a != " + to_string(hspack.a);
       retval   = false;
     }
     if(newpack->b != hspack.b) {
       failmsg += "b != " + hspack.b;
       retval   = false;
     }
     if(newpack->c != hspack.c) {
       failmsg += "c != " + to_string(hspack.c);
       retval   = false;
     }
     if(newpack->d != hspack.d) {
       failmsg += "\nd != " + to_string(hspack.d);
       failmsg += "\ngot "  + to_string(newpack->d) + " instead of " + to_string(hspack.d);
       retval   = false;
     }

     delete newpack;    
     return retval;
}


bool multiple_packstream() {
     opencraft::packets::packet_stream pack_stream;

     opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string("127.0.0.1"),25565,OPENCRAFT_STATE_STATUS);
     opencraft::packets::status_request_status_upstream status_pack;

     opencraft::packets::raw_packet raw_hs(hspack.ident(),hspack.pack());
     opencraft::packets::raw_packet raw_status(status_pack.ident(),status_pack.pack());

     vector<unsigned char> packed_hs     = raw_hs.pack();
     vector<unsigned char> packed_status = raw_status.pack();

     vector<unsigned char> recv_data = std::vector<unsigned char>();

     for(int i=0; i<packed_hs.size(); i++) {
         recv_data.push_back(packed_hs[i]);
     }

     for(int i=0; i<packed_status.size(); i++) {
         recv_data.push_back(packed_status[i]);
     }

     vector<opencraft::packets::raw_packet> inpacks = pack_stream.on_recv(recv_data);

     if(inpacks.size() != 2) {
        failmsg += "pack_stream.on_recv() expected return vector of length 2, got vector of length " + to_string(inpacks.size());
        return false;
     }

     if(inpacks[0].pack_ident != hspack.ident()) {
        failmsg += "packet ident: expected " + to_string(hspack.ident()) + ", got " + to_string(inpacks[0].ident());
        return false;
     }

     if(inpacks[1].pack_ident != status_pack.ident()) {
        failmsg += "packed ident: expected " + to_string(status_pack.ident()) + ", got " + to_string(inpacks[1].ident());
        return false;
     }

     return true;
}



int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;
    
    std::vector<unsigned char> raw_handshake_pack;
    for (int i=0; i<handshake_packet_length; i++) {
        raw_handshake_pack.push_back(handshake_packet[i]);
    }
    cout << endl << "Raw handshake packet from handshake.packet.h:\n" << dump_hex_vector(raw_handshake_pack) << endl << endl;

    run_test("Create/Serialise/Unserialise raw_packet and check fields match",&create_raw);
    run_test("Create/Serialise/Unserialise raw_packet with (ident,data) constructor and check ident matches",&create_raw_ident);
    run_test("Create handshake packet",&create_handshake);
    run_test("Check handshake packet has valid name",&check_handshake_name);
    run_test("Serialise handshake packet returns vector of non-zero length",&serialise_handshake);
    run_test("Unserialising handshake packet returns none-NULL",&unserialise_handshake);
    run_test("Unserialised handshake via constructor has same values as original packet",&unserialise_handshake_via_constructor);
    run_test("Handshake unserialised via unpack_packet() has same values as original packet",&unserialise_handshake_viaunpack_samevalues);
    run_test("Unpack handshake.packet file via handshake packet class constructor does not crash",&unpack_file_via_constructor);
    run_test("Unpacked handshake.packet file has correct field A",&unpack_handshakefile_fielda);
    run_test("Unpacked handshake.packet file has correct field B",&unpack_handshakefile_fieldb);
    run_test("Unpacked handshake.packet file has correct field C",&unpack_handshakefile_fieldc);
    run_test("Unpacked handshake.packet file has correct field D",&unpack_handshakefile_fieldd);
    run_test("127 Packed as varint has same value when unpacked",&packed_127_equal_unpacked);
    run_test("128 Packed as varint has same value when unpacked",&packed_128_equal_unpacked);
    run_test("130 Packed as varint has same value when unpacked",&packed_130_equal_unpacked);
    run_test("Deserialise captured data from minecraft client and read values",&deserialise_captured);
    run_test("Deserialise from a byte stream using full packet write",&simple_packstream);
    run_test("Deserialise multiple packets from a byte stream using full packet write",&multiple_packstream);

    cout << endl;
    cout << tests_passed << "/" << tests_run << " Passed" << endl;
    cout << tests_failed << "/" << tests_run << " Failed" << endl;
    cout << "Pass rate: " << (tests_passed/tests_run)*100.0f << endl;
}




