//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft server.
//
// The OpenCraft server is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft server is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//      OpenCraft server class
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <opencraft_server.h>
#include <opencraft_client_connection.h>

#include <boost/bind.hpp>
#include <list>

opencraft_server::opencraft_server(tcp::endpoint endpoint) {
    this->listen_on = endpoint;
    LOG(debug) << "Will listen on " << endpoint;
}

void opencraft_server::accept_handler(shared_ptr<tcp::socket> client_sock) {
     LOG(info) << "Got a new connection from " << client_sock->remote_endpoint();
     
     opencraft_client_connection::pointer new_conn = opencraft_client_connection::create(this->io_service);

     this->clients.insert(new_conn);
}

void opencraft_server::start_listening() {
     tcp::acceptor _acceptor(this->io_service, this->listen_on);

     shared_ptr<tcp::socket> sock(new tcp::socket(io_service));

     _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
     _acceptor.listen();

     _acceptor.async_accept(*sock, boost::bind(&opencraft_server::accept_handler,this,sock));

     this->io_service.run();
}
