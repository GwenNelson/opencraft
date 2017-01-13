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

void opencraft_server::start_accept() {
     opencraft_client_connection *new_conn = new opencraft_client_connection(io_service_);
     acceptor_.async_accept(new_conn->socket(),
        boost::bind(&opencraft_server::handle_accept, this, new_conn,
          boost::asio::placeholders::error));
}

void opencraft_server::handle_accept(opencraft_client_connection *new_conn,
                                     const boost::system::error_code& error) {
     if(!error) {
        new_conn->start();
     } else {
        delete new_conn;
     }
     start_accept();
}

