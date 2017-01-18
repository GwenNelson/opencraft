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
//     Video mode setup
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <opencraft_video.h>

opencraft_video::opencraft_video(bool fullscreen, unsigned int w, unsigned int h) {
   this->is_fullscreen = fullscreen;
   this->res_w         = w;
   this->res_h         = h;
}

void opencraft_video::init_video() {
     if(SDL_WasInit(0)==0) {
        SDL_Init(0);
     }

     SDL_InitSubSystem(SDL_INIT_TIMER);
     SDL_InitSubSystem(SDL_INIT_VIDEO);
     SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
     SDL_GL_SetSwapInterval(1);

     if(this->is_fullscreen) {
        SDL_DisplayMode disp_mode;
        SDL_GetDesktopDisplayMode(0, &disp_mode);
        this->res_w  = disp_mode.w;
        this->res_h  = disp_mode.h;

        this->screen = SDL_CreateWindow("OpenCraft",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,disp_mode.w,disp_mode.h,
                                                    SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN_DESKTOP);
     } else {

        this->screen = SDL_CreateWindow("OpenCraft",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,this->res_w,this->res_h,
                                                    SDL_WINDOW_OPENGL);
     }
     this->glcontext = SDL_GL_CreateContext(screen); 
}

void opencraft_video::start_frame() {
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void opencraft_video::end_frame() {
     SDL_GL_SwapWindow(this->screen);
}

