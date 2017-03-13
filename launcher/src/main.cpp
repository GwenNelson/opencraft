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

#include <common.h>

#include <stdlib.h>
#include <unistd.h>

#include <nuklear_cpp.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

bool running;

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

int main(int argc, char **argv) {
    int i;
    cout << OPENCRAFT_LAUNCHER_LONG_VER << endl << "Built on " << OPENCRAFT_LAUNCHER_BUILDDATE << endl;
   
    struct nk_context* nk_ctx = init_nuklear("OpenCraft Launcher ");
    running = true;

    while(running) {
       handle_input(nk_ctx);
       ntk_update(nk_ctx);
       render_nuklear();
    }
}
