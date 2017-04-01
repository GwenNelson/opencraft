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

#include <user_profile.h>
#include <opencraft_daemon.h>

#include <boost/random.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 

#include <chrono>
#include <ctime>
#include <cstdint>

#include <libopencraft/proto_constants.h>

#include <tuple>

user_profile::user_profile(std::string _username, opencraft_daemon* d) {
    this->_daemon  = d;
    this->username = _username;
    this->uuid     = boost::uuids::random_generator()(); // TODO - add facility to lookup this in opencraft_daemon
    
    std::time_t now = std::time(0);
    boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};
    
    this->entity_id  = gen();
    this->game_mode  = OPENCRAFT_GAMEMODE_CREATIVE;
    this->dimension  = OPENCRAFT_DIMENSION_OVERWORLD;
    this->difficulty = OPENCRAFT_DIFFICULTY_PEACEFUL;

    std::tuple<int,int,int> spawn_pos = d->get_spawn_pos();
    this->pos_x = (int)std::get<0>(spawn_pos);
    this->pos_y = (int)std::get<1>(spawn_pos);
    this->pos_z = (int)std::get<2>(spawn_pos);

    this->yaw   = 0.0f;
    this->pitch = 0.0f;

    this->on_ground = true;
}
