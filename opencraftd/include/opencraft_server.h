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

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <opencraft_client_connection.h>
#include <set>
#include <map>

using boost::shared_ptr;
using boost::asio::ip::tcp;
using std::set;
using std::map;

class opencraft_server {
   public:
      opencraft_server(boost::asio::io_service& io_service, short port) :
        io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
          start_accept();
        }
      set <shared_ptr<opencraft_client_connection> > clients;
   private:
      void start_accept();
      void handle_accept(opencraft_client_connection *new_conn,
                         const boost::system::error_code& error);
      boost::asio::io_service& io_service_;
      tcp::acceptor acceptor_;
};
