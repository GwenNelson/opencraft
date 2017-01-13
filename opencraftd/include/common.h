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
//     Common includes everything needs to use
//
//-----------------------------------------------------------------------------

#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#define LOG BOOST_LOG_TRIVIAL

typedef enum {HANDSHAKING=0,STATUS=1,LOGIN=2,PLAY=3} proto_mode_t;

#include <packets.autogen.h>
#include <version.h>


