//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft client.
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
//     Console variables
//
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <map>
#include <vector>

#include <common.h>

#include <opencraft_console.h>

#define CVAR_STRING 0
#define CVAR_FLOAT  1
#define CVAR_INT    2
#define CVAR_BOOL   3

class opencraft_cvars {
   public:
     opencraft_cvars();
     void dump_cvars();
     void set_cvar_s(std::string name, std::string val);
     void set_cvar_f(std::string name, float val);
     void set_cvar_i(std::string name, int val);
     void set_cvar_b(std::string name, bool val);
     void toggle_cvar(std::string name);

     std::string get_cvar_s(std::string name);
     float       get_cvar_f(std::string name);
     int         get_cvar_i(std::string name);
     bool        get_cvar_b(std::string name);
   private:
     std::map<std::string, int>         var_types;
     std::map<std::string, std::string> s_vals;
     std::map<std::string, float>       f_vals;
     std::map<std::string, int>         i_vals;
     std::map<std::string, bool>        b_vals;
};

