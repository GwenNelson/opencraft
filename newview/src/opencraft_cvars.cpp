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


#include <opencraft_cvars.h>

opencraft_cvars::opencraft_cvars() {
}

void opencraft_cvars::dump_cvars() {
     std::map<std::string, int>::iterator it;
     std::string s_name;
     for(it = this->var_types.begin(); it != this->var_types.end(); it++) {
         s_name = it->first;
         switch(it->second) {
            case CVAR_STRING: {
               std::string s_val = this->s_vals[s_name];
               console_printf("%s=\"%s\"\n",s_name.c_str(),s_val.c_str());
            break;}
            case CVAR_FLOAT: {
               float f_val = this->f_vals[s_name];
               console_printf("%s=%f\n",s_name.c_str(),f_val);
            break;}
            case CVAR_BOOL: {
               if(this->b_vals[s_name]) {
                 console_printf("%s=True\n",s_name.c_str());
               } else {
                 console_printf("%s=False\n",s_name.c_str());
               } 
            break;}
            case CVAR_INT: {
               int i_val = this->i_vals[s_name];
               console_printf("%s=%d\n",s_name.c_str(),i_val);
            break;}
         }
     }
}

void opencraft_cvars::set_cvar_s(std::string name, std::string val) {
     this->var_types[name] = CVAR_STRING;
     this->s_vals[name]    = val;
}

void opencraft_cvars::set_cvar_f(std::string name, float val) {
     this->var_types[name] = CVAR_FLOAT;
     this->f_vals[name]    = val;
}

void opencraft_cvars::set_cvar_i(std::string name, int val) {
     this->var_types[name] = CVAR_INT;
     this->i_vals[name]    = val;
}

void opencraft_cvars::set_cvar_b(std::string name, bool val) {
     this->var_types[name] = CVAR_BOOL;
     this->b_vals[name]    = val;
}
