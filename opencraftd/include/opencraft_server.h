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

using boost::shared_ptr;
using boost::asio::ip::tcp;

class opencraft_server {
   public:
      opencraft_server(tcp::endpoint endpoint);
      void start_listening();
   private:
      void accept_handler(shared_ptr<tcp::socket> client_sock);
      tcp::endpoint            listen_on;
      boost::asio::io_service  io_service;
      tcp::socket              *server_sock;
};
