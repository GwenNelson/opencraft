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

using std::cerr;
using std::cout;
using std::endl;
using std::string;

namespace po      = boost::program_options;
namespace logging = boost::log;

void configure_logging(bool debug_mode) {
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

    desc.add_options() ("help,h",        "display this help")
                       ("debug,d",       "run in debug mode")
                       ("root,r",         po::value<string>()->default_value("."),
                                         "path to install root")
                       ("texturepack,t",  po::value<string>()->default_value("osrep"),
                                         "Texture pack to use");

    po::variables_map vm;
    
    bool parse_error = false;

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error &error) {
        parse_error = true;
    }

    if(parse_error || vm.count("help")==1) {
       cout << "Usage: newview [options]\n" << desc << endl;
       return 0;
    }

    bool debug_mode = false;      
    if(vm.count("debug")==1) {
       debug_mode = true;
    }

    configure_logging(debug_mode);

    std::string install_root = vm["root"].as<string>();
    std::string texture_pack = vm["texturepack"].as<string>();

    LOG(debug) << "Will try to find texture pack in " << install_root << "/texture_packs/" << texture_pack;

    for(;;);
    return 0;
}
