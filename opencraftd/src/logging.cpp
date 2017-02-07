//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
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
//     Logging stuff
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <libopencraft/version.h>
#include <event2/event.h>

using namespace std;
namespace logging = boost::log;

// this isn't in eventlib's headers for some stupid reason
#define EVENT_DBG_NONE 0
#define EVENT_DBG_ALL 0xffffffffu

void _libevent_fatal_cb(int err) {
     BOOST_LOG_TRIVIAL(fatal) << "Fatal error in libevent: errno=" << errno;
     abort();
}

void libevent_log_cb(int severity, const char *msg) {
     switch (severity) {
        case _EVENT_LOG_DEBUG:
          BOOST_LOG_TRIVIAL(debug) << "libevent: " << string(msg);
        break;
        case _EVENT_LOG_MSG:
          BOOST_LOG_TRIVIAL(info) << "libevent: " << string(msg);
        break;
        case _EVENT_LOG_WARN:
          BOOST_LOG_TRIVIAL(warning) << "libevent: " << string(msg);
        break;
        case _EVENT_LOG_ERR:
          BOOST_LOG_TRIVIAL(error) << "libevent: " << string(msg);
        break;
        default:
        break;
    }
}


void configure_logging(std::string logfile, bool debug_mode) {
     if(!debug_mode) {
       logging::add_file_log(logging::keywords::file_name = logfile,
                             logging::keywords::rotation_size = 10 * 1024 * 1024,
                             logging::keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),
                             logging::keywords::format = "[%TimeStamp%]: %Message%");
     }
     if(debug_mode) {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::debug);
     } else {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
     }
     logging::add_common_attributes();
     event_set_log_callback(libevent_log_cb);
     event_set_fatal_callback(_libevent_fatal_cb);
     event_enable_debug_logging(EVENT_DBG_NONE); // TODO: Make this configurable

}


