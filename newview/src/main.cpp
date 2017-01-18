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

#include <opencraft_video.h>
#include <opencraft_appstate_menu.h>

#include <stdlib.h>
#include <unistd.h>
#include <physfs.h>
#include <SDL.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

namespace po      = boost::program_options;
namespace logging = boost::log;

opencraft_video    *oc_video    = NULL;
opencraft_appstate *oc_appstate = NULL;
bool is_running                 = true;

std::string title_png_path;

void configure_logging(bool debug_mode) {
     if(debug_mode) {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::debug);
     } else {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
     }
     logging::add_common_attributes();
}

void init_vfs(char *argvz, char* install_root) {
     LOG(info) << "Starting VFS layer...";
     if(PHYSFS_init(argvz)==0) {
        LOG(error) << "Could not setup PhysFS - VFS will not be operational!";
        exit(1);
     } else {
        LOG(debug) << "Loaded PhysFS - VFS will be operational";
     }

     if(PHYSFS_setSaneConfig("opencraft","newview","pk3",0,1)==0) {
        LOG(error) << "Error configuring PhysFS!";
        exit(1);
     }

}

void sdl_loop_iter() {
     SDL_Event ev;
     while (SDL_PollEvent(&ev)) {
        switch(ev.type) {
           case SDL_QUIT:
              is_running = false;
           break;
        }
        oc_appstate->update_state(&ev);
     }
}

int main(int argc, char **argv) {
    int i;
    cout << OPENCRAFT_LONG_VER << endl << "Built on " << OPENCRAFT_BUILDDATE << endl;
    po::options_description desc("Options");

    desc.add_options() ("help,h",        "display this help")
                       ("debug,d",       "run in debug mode")
                       ("root,r",         po::value<string>()->default_value("."),
                                         "path to install root")
                       ("width,w",        po::value<unsigned int>()->default_value(1280),
                                         "default window/screen width")
                       ("height,h",       po::value<unsigned int>()->default_value(720),
                                         "default window/screen height")
                       ("fullscreen,f",  "run in fullscreen mode")
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
    bool fullscreen_mode = false;
    if(vm.count("fullscreen")==1) {
       fullscreen_mode = true;
    }

    unsigned int res_w = vm["width"].as<unsigned int>();
    unsigned int res_h = vm["height"].as<unsigned int>();

    configure_logging(debug_mode);

    std::string install_root = vm["root"].as<string>();
    std::string texture_pack = vm["texturepack"].as<string>();

    init_vfs(argv[0], (char*)install_root.c_str());

    LOG(debug) << "Will try to find texture packs in " << install_root << "/texture_packs/";
    std::string full_pack_path = install_root + "/texture_packs";

    if(PHYSFS_mount((const char*)full_pack_path.c_str(),(const char*)"/texture_packs",1)==0) {
       LOG(error) << "Could not mount texture_packs!";
       exit(1);
    } else {
       LOG(debug) << "Mounted " << full_pack_path << " at /texture_packs";
    }
    
    title_png_path = std::string("/texture_packs/") + texture_pack + std::string( "/assets/opencraft/textures/gui/title/opencraft.png");
    if(PHYSFS_exists((const char*)title_png_path.c_str())==0) {
       LOG(error) << "Could not load title image " << title_png_path;
       exit(1);
    }

    LOG(debug) << "Setting video";

    oc_video = new opencraft_video(fullscreen_mode,res_w,res_h);
    oc_video->init_video();

    oc_appstate = new opencraft_appstate_menu();

    while(is_running) {
       oc_video->start_frame();
        sdl_loop_iter();
        oc_appstate->render();
       oc_video->end_frame();
    }
    return 0;
}
