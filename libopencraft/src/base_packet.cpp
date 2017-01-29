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
//     Base packet class
//
//-----------------------------------------------------------------------------


#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <base_packet.h>
#include <raw_packet.h>
#include <arpa/inet.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>


#include <proto_constants.h>
#include <packets.autogen.h>
#include <common.h>

namespace opencraft {
  namespace packets {



int varint_size(int32_t input) {
    if(input==0) return 1;
    int s=0;
    while(input) {
       s++;
       input >>= 7;
    }
    return s;
}


int32_t parse_var_int(unsigned char* buf, size_t buflen) {
        int retval=0;
        for(int i=0; i < 5; ++i) {
            retval |= (buf[i] & 0x7F) << (i*7);
            if(!(buf[i] & 0x80)) {
               break;
            }
        }
        return retval;
}


void unparse_varint(int32_t i, unsigned char *buf) {
     if(!i) {
        *buf=0;
     }

     int n=0;
     while(i) {
        unsigned char p = i & 0x7F;
        i >>= 7;
        if(i) p |= 0x80;
        buf[n++] = p;
     }
}

std::vector<unsigned char> opencraft_packet::pack() {
   std::vector<unsigned char> retval;

   for(int i=0; i< this->packed.size(); i++) {
       retval.push_back(this->packed[i]);
   }

   return retval;
}

opencraft_packet* opencraft_packet::unpack_packet(int proto_state, bool client_bound, std::vector<unsigned char> packdata) {
   raw_packet raw_pack(packdata);
   
   opencraft_packet *retval = NULL;
   #include "unpack_packet.inc" // autogenerated code here - should be a big switch statement

   return retval;
}

uint32_t opencraft_packet::ident() {
   return 0;
}

std::string opencraft_packet::name() {
   return std::string("unknown"); // this should NEVER actually turn up, always override
}

std::string opencraft_packet::dump_hex() {
     std::stringstream retstream;
     for(int i=0; i<this->packed.size(); i++) {
         retstream << std::setfill('0') << std::setw(2) << std::hex << (uint32_t)this->packed[i];
     }
     return retstream.str();
}

unsigned char opencraft_packet::unpack_byte() {
     unsigned char retval = this->packed[this->bufpos];
     this->bufpos++;
     return retval;
}

char opencraft_packet::unpack_char() {
     char retval = (char)this->packed[this->bufpos];
     this->bufpos++;
     return retval;
}

bool opencraft_packet::unpack_boolean() {
     char retval = this->unpack_byte();
     if(retval) return true;
     return false;
}

int32_t opencraft_packet::unpack_varint() {
     unsigned char varint_buf[4];
     int maxsize = 4;
     if(this->packed.size() < 4) maxsize = this->packed.size();
     for(int i=0; i<maxsize; i++) {
         varint_buf[i] = this->packed[this->bufpos+i];
     }
     int32_t retval = parse_var_int(varint_buf,this->packed.size());
     this->bufpos += varint_size(retval);
     return retval;
}

std::string opencraft_packet::unpack_string() {
     int32_t s_len = this->unpack_varint();
     std::string retval;
     for(int i=0; i<s_len; i++) {
         retval.push_back(this->unpack_byte());
     }
     return retval;
}

std::string opencraft_packet::unpack_string16() {
     return this->unpack_string();
}

std::string opencraft_packet::unpack_string8() {
     return this->unpack_string();
}

std::tuple<float,float,float> opencraft_packet::unpack_position() {
     return std::tuple<float,float,float> (this->unpack_float(),this->unpack_float(),this->unpack_float()); // probably incorrect
}

std::vector<unsigned char> opencraft_packet::unpack_bytes() {
     std::vector<unsigned char> retval;
     int32_t s_len = this->packed.size() - this->bufpos;
     for(int i=0; i<s_len; i++) {
         retval.push_back(this->unpack_byte());
     }
     return retval;
}

int32_t opencraft_packet::unpack_enum() {
        return this->unpack_varint();
}

uint16_t opencraft_packet::unpack_short() {
         unsigned char tmpbuf[2];
         for(int i=0; i<2; i++) {
             tmpbuf[i] = this->packed[this->bufpos+i];
         }
         this->bufpos+=2;
 
         uint16_t retval = ntohs(*(uint16_t*)tmpbuf);
         return retval;
}

void opencraft_packet::pack_boolean(bool val) {
     if(val) {
        this->pack_byte((unsigned char)1);
     } else {
        this->pack_byte((unsigned char)0);
     }
}
void opencraft_packet::pack_byte(unsigned char val) {
     this->packed.push_back(val);
}
void opencraft_packet::pack_char(char val) {
     this->packed.push_back((unsigned char)val);
}

void opencraft_packet::pack_string(std::string val){
     this->pack_varint(val.size());
     unsigned char *s = (unsigned char*)val.c_str();
     for(int i=0; i<val.size(); i++) {
         this->pack_byte(s[i]);
     }
}
void opencraft_packet::pack_string16(std::string val) {
     this->pack_string(val);
}

void opencraft_packet::pack_double(double val){}
void opencraft_packet::pack_float(float val){}
void opencraft_packet::pack_int(int32_t val){}
void opencraft_packet::pack_long(int64_t val){}
void opencraft_packet::pack_short(int16_t val) {
     uint16_t netval = val;
     this->packed.push_back((unsigned char)(netval >> 8));
     this->packed.push_back((unsigned char)(netval &  0xFF));
}
void opencraft_packet::pack_string8(std::string val){}
void opencraft_packet::pack_varint(int32_t val) {
     unsigned char buf[4];
     unparse_varint(val,buf);
     for(int i=0; i<varint_size(val); i++) {
         this->packed.push_back(buf[i]);
     }
}
void opencraft_packet::pack_position(std::tuple<float,float,float>){}
void opencraft_packet::pack_enum(int32_t val){}
void opencraft_packet::pack_bytes(std::vector<unsigned char> data) {
     for(int i=0; i<data.size(); i++) {
         this->pack_byte(data[i]);
     }
}

double  opencraft_packet::unpack_double() {this->bufpos = this->bufpos + sizeof(double);return 0;}    // dumb placeholder for now
float   opencraft_packet::unpack_float()  {this->bufpos = this->bufpos + sizeof(float); return 0.0f;} // dumb placeholder for now
int32_t opencraft_packet::unpack_int()    {this->bufpos+=4; return 0;}                 // dumb placeholder for now
int64_t opencraft_packet::unpack_long()   {this->bufpos+=8; return 0;}                 // dumb placeholder for now

}
}
