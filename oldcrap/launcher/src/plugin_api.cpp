//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of the OpenCraft launcher.
//
// The OpenCraft client is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// The OpenCraft launcher is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenCraft.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION:
//          Implementation of the functions in plugin_api.h
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <plugin_api.h>

std::string read_url(std::string url) {
   auto r = cpr::Get(cpr::Url{url});
   if (r.status_code != 200) {   // TODO: make this handle weird 2xx codes that are technically actually valid
      LOG(error) << "Error " << r.status_code << " while reading " << url;
      return NULL;
   }
   return r.text;
}

Json::Value json_from_string(std::string s) {
      Json::Value retval;
      Json::Reader reader;
      bool parsed_ok = reader.parse(s.c_str(), retval);
      if(parsed_ok) return retval;

      LOG(error) << "Error parsing JSON string: " << reader.getFormattedErrorMessages();
      return retval;
}
