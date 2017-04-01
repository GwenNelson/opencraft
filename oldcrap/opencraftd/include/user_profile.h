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
//     Class representing a user profile
//
//-----------------------------------------------------------------------------

#pragma once

#include <boost/uuid/uuid.hpp>

#include <string>

class opencraft_daemon;

class user_profile {
   public:
      user_profile(std::string _username, opencraft_daemon *d);
      std::string username;
      boost::uuids::uuid uuid;

      double pos_x;
      double pos_y;
      double pos_z;
      float  yaw;
      float  pitch;
      bool   on_ground;
      
      int entity_id;
      int dimension;

      unsigned char difficulty;
      unsigned char game_mode;
      
      opencraft_daemon* _daemon;
};
