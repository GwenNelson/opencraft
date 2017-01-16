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


#include <common.h>

#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>

#include <event_types.autogen.h>
#include <opencraft_event_dispatcher.h>

opencraft_event_dispatcher::opencraft_event_dispatcher(boost::asio::io_service &__io_service) {
     this->_io_service = &__io_service;
}

void opencraft_event_dispatcher::fire_event(int32_t event_id, void *event_data) {
     
}

void opencraft_event_dispatcher::register_handler(int32_t event_id, event_callback_t cb) {
     this->event_callbacks[event_id].push_back(cb);
}

