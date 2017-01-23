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
//     Various constants used by the protocol
//
//-----------------------------------------------------------------------------

#pragma once

#define OPENCRAFT_PROTOCOL_VERSION 315
#define OPENCRAFT_DEFAULT_SERVER   "127.0.0.1:25565"
#define OPENCRAFT_DEFAULT_TCP_PORT 25565

#define OPENCRAFT_STATE_HANDSHAKING 0
#define OPENCRAFT_STATE_STATUS      1
#define OPENCRAFT_STATE_LOGIN       2
