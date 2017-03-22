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
//     C++ wrapper for nuklear
//
//-----------------------------------------------------------------------------

#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <opencraft/common.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION

#include <opencraft/nuklear_gui.h>

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

using namespace std;

opencraft::nuklear::app::app(string window_title, int win_width, int win_height) {
    this->_win_title  = window_title;
    this->_win_width  = win_width;
    this->_win_height = win_height;
    this->running     = false;
}

void opencraft::nuklear::app::init() {
     SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
     SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS);
     SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
     SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
     SDL_GL_SetSwapInterval(1);
     this->sdl_window = SDL_CreateWindow(this->_win_title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        this->_win_width, this->_win_height, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
     this->glContext = SDL_GL_CreateContext(sdl_window);
     SDL_GetWindowSize(this->sdl_window, &(this->_win_width), &(this->_win_height));

     glViewport(0, 0, this->_win_width, this->_win_height);
     glewExperimental = 1;
     if (glewInit() != GLEW_OK) {
         fprintf(stderr, "Failed to setup GLEW\n");
         exit(1);
     }

     this->background = nk_rgb(28,48,62); // TODO: make this a param
     float bg[4];
     nk_color_fv(bg, this->background);
     glClearColor(bg[0], bg[1], bg[2], bg[3]);

     this->ctx = nk_sdl_init(this->sdl_window);
     struct nk_font_atlas *atlas;
     nk_sdl_font_stash_begin(&atlas);
     nk_sdl_font_stash_end();
     this->running = true;
}

void opencraft::nuklear::app::iter_loop() {
     if(!this->running) {
        exit(0);
     }

     SDL_Event e;
     nk_input_begin(this->ctx);
     while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
           running=false;
           return; // do nothing more
        } else {
            nk_sdl_handle_event(&e);
        }
     }
     nk_input_end(this->ctx);

     if(nk_begin(this->ctx,"",nk_rect(0,this->_win_height-20,this->_win_width,this->_win_height),NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_dynamic(this->ctx, 14, 1);
        nk_label(this->ctx, opencraft_framework_longver(),NK_TEXT_LEFT);
     }
     nk_end(this->ctx);

     this->draw_gui();

     glViewport(0,0,this->_win_width,this->_win_height);
     glClear(GL_COLOR_BUFFER_BIT);
     nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
     SDL_GL_SwapWindow(this->sdl_window);
}
