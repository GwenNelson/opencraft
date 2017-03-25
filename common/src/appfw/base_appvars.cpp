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
    this->set(val);
}

std::string BaseAppVar::get() {
    return this->s_val;
}

void BaseAppVar::set(std::string s) {
     this->s_val = s;
}

BaseAppVars::BaseAppVars() {
}

std::string BaseAppVars::get(std::string k) {
    if(this->vars.find(k) != this->vars.end()) {
       return this->vars[k]->get();
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


}}}};
