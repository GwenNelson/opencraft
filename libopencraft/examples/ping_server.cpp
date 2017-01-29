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
//     Connects to a server, pings it and displays server info
//     This example does everything manually
//     Most is shamelessly ripped from the boost asio examples
//
//-----------------------------------------------------------------------------


#include <libopencraft/packets.autogen.h>
#include <libopencraft/version.h>
#include <libopencraft/proto_constants.h>
#include <libopencraft/raw_packet.h>
#include <libopencraft/packet_stream.h>
#include <iostream>

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std;

using boost::asio::ip::tcp;


int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;
   
    // setup all the ASIO crap
    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query("localhost", "25565");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

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


   cout << "Creating a handshake packet..." << endl;
   // create a handshake packet
   opencraft::packets::handshake_handshaking_upstream hspack(OPENCRAFT_PROTOCOL_VERSION,std::string("127.0.0.1"),25565,OPENCRAFT_STATE_STATUS);

   cout << "Hex dump of handshake packet: " << hspack.dump_hex() << endl;

   // wrap it in a raw packet
   cout << "Wrapping handshake inside raw packet..." << endl;

   opencraft::packets::raw_packet raw_hs(hspack.ident(),hspack.pack());

   cout << "Hex dump of raw packet containing handshake packet: " << raw_hs.dump_hex() << endl;

   cout << "Transmitting..." << endl;

   // transmit it
   boost::system::error_code net_error;
   boost::asio::write(socket, boost::asio::buffer(raw_hs.pack()),boost::asio::transfer_all(), net_error);

   if(net_error) {
      throw boost::system::system_error(net_error);
   }
   
   cout << "Creating a status request packet..." << endl;
   opencraft::packets::status_request_status_upstream status_pack;
   cout << "Hex dump of status request packet: " << status_pack.dump_hex() << endl;
   
   cout << "Wrapping status request inside raw packet..." << endl;
   opencraft::packets::raw_packet raw_status(status_pack.ident(),status_pack.pack());

   cout << "Hex dump of raw packet containing status request: " << raw_status.dump_hex() << endl;

   cout << "Transmitting..." << endl;
   boost::asio::write(socket, boost::asio::buffer(raw_status.pack()),boost::asio::transfer_all(), net_error);

   cout << "Receiving..." << endl;
   std::vector<unsigned char> indata = std::vector<unsigned char>(4096);
   size_t bytes_read;


   opencraft::packets::packet_stream pack_stream;
   bytes_read = boost::asio::read(socket, boost::asio::buffer(indata,4096), boost::asio::transfer_at_least(16));

   vector<opencraft::packets::raw_packet> inpacks = pack_stream.on_recv(indata);

   // we (naively) assume there's only one packet and that it's a status response
   opencraft::packets::status_response_status_downstream *newpack = (opencraft::packets::status_response_status_downstream*)opencraft::packets::opencraft_packet::unpack_packet(OPENCRAFT_STATE_STATUS,true,inpacks[0].pack());

   cout << "Here is the JSON data from the server:\n" << newpack->a << endl;
}




