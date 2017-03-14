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
//          Main entry point
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>

#include <stdlib.h>
#include <unistd.h>

#include <nuklear_cpp.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <glob.h>
#include <dlfcn.h>
#include <libgen.h>

#include <common.h>
#include <plugin_api.h>

#include <map>
#include <vector>

using namespace std;
using namespace boost::filesystem;

namespace logging = boost::log;

bool running;
bool debug;

map<string,module_info_t*> loaded_modules_info;
map<string,void*>          loaded_modules_handles;
map<string,void*>          loaded_modules_apis;
vector<string>             loaded_client_modules;
vector<string>             loaded_server_modules;

void configure_logging(bool debug_mode) {
     if(debug_mode) {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::debug);
     } else {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
     }
     logging::add_common_attributes();
}

void handle_input(struct nk_context* ctx) {
     SDL_Event e;
     nk_input_begin(ctx);
     while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
           running=false;
           return; // do nothing more
        } else {
            nk_sdl_handle_event(&e);
        }
     }
     nk_input_end(ctx);
}

void ntk_update(struct nk_context* ctx) {
     if (nk_begin(ctx, "OpenCraft Launcher", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),0)) {
         nk_menubar_begin(ctx);
         nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
         nk_layout_row_push(ctx, 45);
         if (nk_menu_begin_label(ctx, "File", NK_TEXT_LEFT, nk_vec2(160, 200))) {
             nk_layout_row_dynamic(ctx, 12, 1);
             nk_menu_item_label(ctx, "Updates:", NK_TEXT_LEFT);
             nk_menu_item_label(ctx, "  Check for updates", NK_TEXT_LEFT);
             nk_menu_item_label(ctx, "  Install all updates", NK_TEXT_LEFT);
             nk_menu_end(ctx);
         }
         nk_layout_row_push(ctx, 60);
         if (nk_menu_begin_label(ctx, "Profile", NK_TEXT_LEFT, nk_vec2(160, 200))) {
             nk_layout_row_dynamic(ctx, 14, 1);
             nk_menu_item_label(ctx, "Select profile:", NK_TEXT_LEFT);
             nk_menu_item_label(ctx, "  Default", NK_TEXT_LEFT);
             nk_menu_item_label(ctx, "Edit profiles", NK_TEXT_LEFT);
             nk_menu_end(ctx);
         }
         nk_layout_row_end(ctx);
         nk_menubar_end(ctx);
     }
     nk_end(ctx);
}

void run_gui() {
    struct nk_context* nk_ctx = init_nuklear("OpenCraft Launcher ");
    running = true;

    while(running) {
       handle_input(nk_ctx);
       ntk_update(nk_ctx);
       render_nuklear();
    }

}

void load_plugin(std::string filename) {
     LOG(debug) << "Attempting to load " << filename;
     void* handle=dlopen(filename.c_str(),RTLD_LAZY);

     if(handle==NULL) {
        LOG(error) << "Failed to load " << filename << ", dlerror() returned " << std::string(dlerror());
        return;
     }
     
     module_info_t* mod_info = (module_info_t*)dlsym(handle, "module_info");
     if(mod_info==NULL) {
        LOG(error) << "Missing module_info in " << filename << ", dlerror() returned " << std::string(dlerror());
        dlclose(handle);
        return;
     }

     if(mod_info->init_module != NULL) {
        LOG(debug) << "Initializing module " << filename;
        mod_info->init_module(); // this must be called if defined so that plugins can setup their structs and tables
     }

     if(mod_info->check_for_update != NULL) { // TODO: move this update check to somewhere else or stick it into a background thread so we don't lag like mad at startup
        LOG(debug) << "Checking for available update for module " << filename;
        if(mod_info->check_for_update()) {
          LOG(info) << "Update is available for module " << filename; // TODO: implement getting latest version, changelog and other crap
        } else {
          LOG(info) << "No available updates for module " << filename;
        }
     } else {
        LOG(debug) << "Automatic updates not supported for module " << filename;
     }

     std::string base_filename = std::string(basename((char*)filename.c_str()));
     void* api_ptr = NULL;

     if(mod_info->module_type == MODTYPE_CLIENT) {
        LOG(info) << "Loaded client module " << base_filename << ": " << mod_info->module_name << ", " << mod_info->module_copyright;
        LOG(info) << mod_info->module_name << ": Version " << mod_info->module_version;
        api_ptr = dlsym(handle,"client_api");

        if(api_ptr==NULL) {
           LOG(error) << "Missing client_api in " << filename << ", dlerror() returned " << std::string(dlerror());
           dlclose(handle); // without an API we can't do anything, so close and return
           return;
        }
        loaded_client_modules.push_back(base_filename);
     } else if(mod_info->module_type == MODTYPE_SERVER) {
        LOG(info) << "Loaded server module " << base_filename << ": " << mod_info->module_name << ", " << mod_info->module_copyright;
        LOG(info) << mod_info->module_name << ": Version " << mod_info->module_version;
        api_ptr = dlsym(handle,"server_api");

        if(api_ptr==NULL) {
           LOG(error) << "Missing server_api in " << filename << ", dlerror() returned " << std::string(dlerror());
           dlclose(handle); // without an API we can't do anything, so close and return
           return;
        }

        loaded_server_modules.push_back(base_filename);
     } else {
        LOG(error) << "Unknown module type, unloading " << base_filename;
        dlclose(handle);
        return;
     }
     loaded_modules_info[base_filename]    = mod_info;
     loaded_modules_handles[base_filename] = handle;
     loaded_modules_apis[base_filename]    = api_ptr;
}

void load_plugins() {
     LOG(info) << "Searching for plugins...";

     glob_t glob_result;
     glob("plugins/*.so",GLOB_TILDE,NULL,&glob_result); // TODO: make this cross-platform - e.g dylib on OS X, dll on that shitty OS we all know and hate
     unsigned int i;
     for(i=0; i<glob_result.gl_pathc; ++i){
         LOG(debug) << "Found plugin file " << glob_result.gl_pathv[i];
         load_plugin(std::string(glob_result.gl_pathv[i]));
     }
     globfree(&glob_result);
     LOG(info) << "Located " << i << " plugins";
}

void update_versions() {
     LOG(info) << "Updating client+server version information...";

     for (auto& kv : loaded_modules_info) {
         std::string plugin_filename = kv.first;
         module_info_t* mod_info     = kv.second;

         if(mod_info->module_type == MODTYPE_CLIENT) {
           client_api_t *client_api = (client_api_t*)loaded_modules_apis[plugin_filename];
           if(client_api->update_version_data != NULL) {
              client_api->update_version_data();
           }
         } else if (mod_info->module_type == MODTYPE_SERVER) {
           server_api_t *server_api = (server_api_t*)loaded_modules_apis[plugin_filename];
           if(server_api->update_version_data != NULL) {
              server_api->update_version_data();
           }

         }
         
     }
}

int main(int argc, char **argv) {
    int i;
    cout << OPENCRAFT_LAUNCHER_LONG_VER << endl << "Built on " << OPENCRAFT_LAUNCHER_BUILDDATE << endl;

    debug = true;

    configure_logging(debug);

    load_plugins();   

    update_versions();
}
