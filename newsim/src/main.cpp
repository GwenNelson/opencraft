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
//          Main entry point and options stuff
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <common.h>
#include <opencraft_daemon.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

namespace po      = boost::program_options;
namespace logging = boost::log;

void shutdown_server();
void startup_server();

void configure_logging(std::string logfile, bool debug_mode) {
     logging::add_file_log(logging::keywords::file_name = logfile,
                           logging::keywords::rotation_size = 10 * 1024 * 1024,
                           logging::keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),
                           logging::keywords::format = "[%TimeStamp%]: %Message%"); 
     if(debug_mode) {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::debug);
     } else {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
     }
     logging::add_common_attributes();
}

int main(int argc, char **argv) {
    int i;
    cout << OPENCRAFT_LONG_VER << endl << "Built on " << OPENCRAFT_BUILDDATE << endl;
    po::options_description desc("Options");

    desc.add_options() ("help,h",       "display this help")
                       ("foreground,f", "don't daemonize")
                       ("debug,d",      "run in debug mode")
                       ("workers,w",    po::value<int>()->default_value(4),
                                        "how many worker threads to use in the main threadpool")
                       ("logfile,l",    po::value<string>()->default_value("./opencraftd.log.%N"),
                                        "log to file")
                       ("root,r",       po::value<string>()->default_value("."),
                                        "path to install root")
                       ("pidfile,p",    po::value<string>()->default_value("~/.opencraftd.pid"),
                                        "path to the PID file for daemon");

    po::variables_map vm;
    
    bool parse_error = false;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error &error) {
        parse_error = true;
    }

    if(parse_error || vm.count("help")==1) {
       cout << "Usage: opencraftd [options]\n" << desc << endl;
       return 0;
    }

    bool debug_mode = false;      
    if(vm.count("debug")==1) {
       debug_mode = true;
    }

    bool daemon_mode = true;
    if(vm.count("foreground")==1) {
       daemon_mode = false;
    }

    std::string logfilename = vm["logfile"].as<string>();
    configure_logging(logfilename,debug_mode);

    int         thread_count = vm["workers"].as<int>();
    std::string install_root = vm["root"].as<string>();
    std::string pidfile      = vm["pidfile"].as<string>();

    opencraft_daemon daemon(debug_mode, daemon_mode, thread_count, pidfile, install_root);
    daemon.run();

    /*if(daemon_mode) {
       LOG(info) << "Daemonizing...";
       write_pidfile(vm["pidfile"].as<string>());
       setup_daemon();
       configure_daemon_stdio();
       configure_daemon_signals();
    }

    startup_server();*/
    for(;;);
    return 0;
}
