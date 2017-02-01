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
//     Uses spawning_client to implement a simple chat client
//
//-----------------------------------------------------------------------------


#include <libopencraft/packets.autogen.h>
#include <libopencraft/version.h>
#include <libopencraft/proto_constants.h>
#include <libopencraft/spawning_client.h>

#include <iostream>

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std;

using boost::asio::ip::tcp;

int proto_mode = OPENCRAFT_STATE_LOGIN;

void chat_cb(opencraft::packets::chat_message_play_downstream *pack) {
     cout << "Got chat: " << pack->a << endl;
}

void login_cb(opencraft::packets::login_success_login_downstream *pack) {
     cout << "Logged in!" << endl;
     proto_mode = OPENCRAFT_STATE_PLAY;
}

int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;
   
    // setup all the ASIO crap
    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query("localhost", "25565");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // setup our client
    opencraft::client::basic_client oc_client;

    // connect
    cout << "Connecting to 127.0.0.1:25565..." << endl;
    tcp::socket socket(io_service);

    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
      throw boost::system::system_error(error);

    boost::system::error_code net_error;


    // send a handshake
    boost::asio::write(socket,boost::asio::buffer(oc_client.send_hs("127.0.0.1",25565,OPENCRAFT_STATE_LOGIN)),boost::asio::transfer_all(), net_error);

    // send a login start
    opencraft::packets::login_start_login_upstream login_start_pack("TestUser");
    boost::asio::write(socket,boost::asio::buffer(oc_client.send_pack(&login_start_pack)),boost::asio::transfer_all(), net_error);

   
        // receive packets and trigger callbacks
        std::vector<unsigned char> indata = std::vector<unsigned char>(1024);
        size_t bytes_read;
    opencraft::packets::packet_stream pack_stream;
   while(true) {
        std::vector<unsigned char> indata = std::vector<unsigned char>(1024);
        bytes_read = boost::asio::read(socket, boost::asio::buffer(indata,1024), boost::asio::transfer_at_least(1));
        vector<opencraft::packets::raw_packet> inpacks = pack_stream.on_recv(indata);
 
        opencraft::packets::opencraft_packet *inpack = NULL;
        for(int i=0; i < inpacks.size(); i++) {
            inpack = opencraft::packets::opencraft_packet::unpack_packet(proto_mode,true,inpacks[i].pack());
            if(inpack != NULL) {
               if(proto_mode == OPENCRAFT_STATE_LOGIN) {
                  if(inpack->ident() == OPENCRAFT_PACKIDENT_LOGIN_SUCCESS_LOGIN_DOWNSTREAM) login_cb(inpack);
               }
               if(proto_mode == OPENCRAFT_STATE_PLAY) {
                  if(inpack->ident() == OPENCRAFT_PACKIDENT_CHAT_MESSAGE_PLAY_DOWNSTREAM) chat_cb(inpack);
               }
               cout << inpack->name() << endl;
            }
            delete inpack;
        }
   }


}




