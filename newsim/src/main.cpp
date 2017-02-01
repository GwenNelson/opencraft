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
//     Entry point for the newsim server
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <opencraft_daemon.h>
#include <libopencraft/version.h>


opencraft_daemon *oc_daemon = NULL;

using namespace std;
namespace po      = boost::program_options;
namespace logging = boost::log;

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
}


int main(int argc, char** argv) {
    cout << LIBOPENCRAFT_LONG_VER << endl << "Built on " << LIBOPENCRAFT_BUILDDATE << endl << endl;

    po::options_description desc("Options");

    desc.add_options() ("help,h",       "display this help")
                       ("foreground,f", "don't daemonize")
                       ("debug,d",      "run in debug mode")
                       ("logfile,l",    po::value<string>()->default_value("./opencraftd.log.%N"),
                                        "log to file")
                       ("port,p",       po::value<int>()->default_value(25565),
                                        "port to listen on")
                       ("root,r",       po::value<string>()->default_value("."),
                                        "path to install root");

    po::variables_map vm;
    
    bool parse_error = false;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error &error) {
        parse_error = true;
    }

    if(parse_error || vm.count("help")==1) {
       cout << "Usage: " << argv[0] << " [options]\n" << desc << endl;
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

    std::string install_root = vm["root"].as<string>();
    std::string pidfile      = std::string("~/.opencraftd.pid");

    int listen_port = vm["port"].as<int>();

    oc_daemon = new opencraft_daemon(debug_mode, daemon_mode, pidfile, install_root, listen_port);
    oc_daemon->run();

    for(;;) sleep(1);
}
