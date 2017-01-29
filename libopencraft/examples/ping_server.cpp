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


#include <packets.autogen.h>
#include <version.h>
#include <proto_constants.h>
#include <handshake.packet.h>
#include <raw_packet.h>
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
   boost::system::error_code ignored_error;
   boost::asio::write(socket, boost::asio::buffer(raw_hs.pack()),boost::asio::transfer_all(), ignored_error);
   
   cout << "Creating a status request packet..." << endl;
   opencraft::packets::status_request_status_upstream status_pack;
   cout << "Hex dump of status request packet: " << status_pack.dump_hex() << endl;
   
   cout << "Wrapping status request inside raw packet..." << endl;
   opencraft::packets::raw_packet raw_status(status_pack.ident(),status_pack.pack());

   cout << "Hex dump of raw packet containing status request: " << raw_status.dump_hex() << endl;

   cout << "Transmitting..." << endl;
   boost::asio::write(socket, boost::asio::buffer(raw_status.pack()),boost::asio::transfer_all(), ignored_error);

}




