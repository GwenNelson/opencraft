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

void chat_cb(void* ctx, opencraft::packets::chat_message_play_downstream *pack) {
     cout << "Got chat: " << pack->a << endl;
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
    opencraft::client::spawning_client oc_client("TestUser");
    oc_client.register_handler(OPENCRAFT_PACKIDENT_CHAT_MESSAGE_PLAY_DOWNSTREAM,chat_cb,NULL);

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

    // we basically just sit here in a loop, transmitting and receiving
   while(true) { 
        boost::system::error_code net_error;
        boost::asio::write(socket, boost::asio::buffer(oc_client.on_send()),boost::asio::transfer_all(), net_error);

        if(net_error) {
           throw boost::system::system_error(net_error);
        }
   
        // receive packets and trigger callbacks
        std::vector<unsigned char> indata = std::vector<unsigned char>(4096);
        size_t bytes_read;
        opencraft::packets::packet_stream pack_stream;

        bytes_read = boost::asio::read(socket, boost::asio::buffer(indata,4096), boost::asio::transfer_at_least(2));
        oc_client.on_recv(indata);
        indata.clear();
   }


}




