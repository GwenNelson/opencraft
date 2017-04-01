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
//     Background client
//
//-----------------------------------------------------------------------------

#pragma once

#include <libopencraft/base_packet.h>
#include <libopencraft/packet_reader.h>
#include <libopencraft/packet_writer.h>

class bg_client {
   public:
      bg_client(std::string _server_addr, int _server_port, std::string _username);
      int proto_mode;
      bool active;
      opencraft::packets::packet_reader *client_reader;
      opencraft::packets::packet_writer *client_writer;
   private:
      void reader_thread();
      std::string server_addr;
      int server_port;
      std::string username;
      int  sockfd;
};

