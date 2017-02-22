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
//     Console
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <opencraft_console.h>
#include <opencraft_video.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

extern opencraft_video *oc_video;

#include <oglconsole.h>

#include <iostream>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string>

namespace logging = boost::log;

void cmd_cb(OGLCONSOLE_Console console, char* cmd) {
     LOG(debug) << "Got console input: " << cmd;
}

opencraft_console::opencraft_console() {
   unsigned int res_w = oc_video->res_w;
   unsigned int res_h = oc_video->res_h;
   glViewport(0,0, (GLsizei)res_w, (GLsizei)res_h);
   OGLCONSOLE_Create();
   OGLCONSOLE_EnterKey(cmd_cb);

   this->stream = new std::ostringstream();

   boost::shared_ptr< logging::core > core = logging::core::get();
   boost::shared_ptr< logging::sinks::text_ostream_backend > backend =
        boost::make_shared< logging::sinks::text_ostream_backend >();
   backend->add_stream(boost::shared_ptr<std::ostream>(this->stream));
   backend->add_stream(boost::shared_ptr<std::ostream>(&std::cout));

   backend->auto_flush(true);

   typedef logging::sinks::synchronous_sink< logging::sinks::text_ostream_backend > sink_t;

   boost::shared_ptr< sink_t > sink(new sink_t(backend));
   core->add_sink(sink);
}

bool opencraft_console::is_active() {
     return this->active;
}

void opencraft_console::toggle() {
//     this->active = (not this->active);
//     OGLCONSOLE_SetVisibility((int)this->active);
}

void opencraft_console::render() {
     if(this->stream->str().size() > 0) {
        OGLCONSOLE_Print((char*)this->stream->str().c_str());
        this->stream->str("");
     }
     OGLCONSOLE_Draw();
}
