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
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//     Event dispatcher C++ side
//
//-----------------------------------------------------------------------------

#pragma once

#include <common.h>

#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>

#include <event_types.autogen.h>

#include <map>
#include <vector>
#include <string>

#include <boost/python.hpp>
#include <frameobject.h>

typedef void (*event_callback_t)(int32_t event_id, void *event_data);

typedef struct event_data_onpacket_t {
   boost::python::object *client_conn; // the python client connection
   std::string data;                   // raw packet data
} event_data_onpacket_t;

class opencraft_event_dispatcher {
   public:
     opencraft_event_dispatcher(boost::asio::io_service &__io_service);
     void fire_event(int32_t event_id, void *event_data);
     void register_handler(int32_t event_id, event_callback_t cb);
   private:
     std::map<int32_t,std::vector<event_callback_t> > event_callbacks;
     boost::asio::io_service *_io_service;
};
