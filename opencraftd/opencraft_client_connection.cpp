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
//      OpenCraft client connection class - sends and receives packets
//
//-----------------------------------------------------------------------------

#include <cstdlib>
#include <memory>
#include <utility>

#include <common.h>
#include <opencraft_client_connection.h>

void opencraft_client_connection::start() {
     LOG(info) << "Started connection handling!";
     do_read();
}

void opencraft_client_connection::do_read() {
     auto self(shared_from_this());
     _socket.async_read_some(boost::asio::buffer(_data, 1024),
         [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                pending_recv.insert(pending_recv.end(), _data, _data+length);
            }
         });
}
