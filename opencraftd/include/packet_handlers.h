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
//     Packet handler functions - where the magic happens
//
//-----------------------------------------------------------------------------

#pragma once

#include <common.h>
#include <bbuff.h>

#include <stdint.h>

#include <opencraft_client_connection.h>

void handle_handshake(void* client,int32_t packlen); 

void handle_status_request(void* client, int32_t packlen);
void handle_status_ping(void* client, int32_t packlen);

void handle_login_start(void* client,int32_t packlen);
