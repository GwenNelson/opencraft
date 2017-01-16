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
//     Entity data - simple base class for all entities
//
//-----------------------------------------------------------------------------

#pragma once

#include <common.h>

#include <opencraft_daemon.h>
#include <opencraft_event_dispatcher.h>

#include <string>

// based on the classes here - http://minecraft.gamepedia.com/Data_values/Entity_IDs
// no, this would NOT be better as a set of actual C++ classes - i know what i'm doing bitches
typedef enum {
  ENTITY_CLASS_DROP        = 0,
  ENTITY_CLASS_IMMOBILE    = 1,
  ENTITY_CLASS_PROJECTILE  = 2,
  ENTITY_CLASS_BLOCK       = 3,
  ENTITY_CLASS_VEHICLE     = 4,
  ENTITY_CLASS_HOSTILE_MOB = 5,
  ENTITY_CLASS_PASSIVE_MOB = 6,
  ENTITY_CLASS_PLAYER      = 7
} entity_class_t;

class opencraft_entity_data {
   public:
     entity_class_t e_class;
     // TODO add position, UUID, entity ID and all that stuff
     virtual void tick() = 0;
};
