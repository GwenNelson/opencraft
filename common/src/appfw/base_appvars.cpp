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
//     BaseAppVars class
//
//-----------------------------------------------------------------------------


#include <opencraft/common.h>

#include <map>
#include <string>

#include <opencraft/appfw/appstate/appvars/base_appvars.h>


namespace opencraft { namespace appfw {
   namespace appstate { namespace appvars {

BaseAppVar::BaseAppVar(std::string val) {
    this->val_type = APPVAR_STR;
    this->set(val);
}

BaseAppVar::BaseAppVar(uint64_t val) {
    this->val_type = APPVAR_INT;
    this->set(val);
}

BaseAppVar::BaseAppVar(bool val) {
    this->val_type = APPVAR_BOOL;
    this->set(val);
}

std::string BaseAppVar::get_str() {
    if(this->val_type == APPVAR_STR) {
       return this->s_val;
    } else {
       if(this->val_type == APPVAR_INT) return std::to_string(this->i_val);
       if(this->val_type == APPVAR_BOOL) {
          if(this->b_val) return "true";
          return "false";
       }
    }
}

void BaseAppVar::set(std::string s) {
     this->val_type = APPVAR_STR;
     this->s_val = s;
}

void BaseAppVar::set(uint64_t i) {
     this->val_type = APPVAR_INT;
     this->i_val = i;
}

void BaseAppVar::set(bool b) {
     this->val_type = APPVAR_BOOL;
     this->b_val = b;
}

BaseAppVars::BaseAppVars() {
}

std::string BaseAppVars::get_str(std::string k) {
    if(this->vars.find(k) != this->vars.end()) {
       return this->vars[k]->get_str();
    }
    return "";
}

void BaseAppVars::set(std::string k, std::string v) {
     if(this->vars.find(k) != this->vars.end()) {
        this->vars[k]->set(v);
        return;
     }
     this->vars[k] = new BaseAppVar(v);
}

void BaseAppVars::set(std::string k, uint64_t v) {
     if(this->vars.find(k) != this->vars.end()) {
        this->vars[k]->set((uint64_t)v);
        return;
     }
     this->vars[k] = new BaseAppVar(v);
}

void BaseAppVars::set(std::string k, bool v) {
     if(this->vars.find(k) != this->vars.end()) {
        this->vars[k]->set(v);
        return;
     }
     this->vars[k] = new BaseAppVar((bool)v);
}



}}}};
