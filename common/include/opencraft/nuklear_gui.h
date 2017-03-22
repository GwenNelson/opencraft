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
//     C++ wrapper and app framework for nuklear
//     Based heavily on the demo code that comes with Nuklear
//
//-----------------------------------------------------------------------------

#include <opencraft/common.h>

#include <opencraft/nuklear.h>
#include <opencraft/nuklear_sdl_gl3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <string>

// simple usage instructions:
//  1 - create subclass of app class
//  2 - override draw_gui() with nuklear commands reflecting current app state
//  3 - call init() to spawn the window
//  4 - run iter_loop() repeatedly, if the user closes the main window it'll cleanup and close the program for you

// to handle button presses etc, see the nuklear examples

namespace opencraft { namespace nuklear {
  class app {
     public:
       app(std::string window_title, int win_width, int win_height);
       void init();
       void iter_loop();

       // OVERRIDE THIS STUFF
       virtual void draw_gui() = 0;
     protected:
        bool               running;
        SDL_Window         *sdl_window;
        SDL_GLContext      glContext;
        std::string        _win_title;
        int                _win_width;
        int                _win_height;
        struct nk_color    background;
        struct nk_context  *ctx;
  };
}}
