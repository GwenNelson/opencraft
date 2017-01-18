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

#pragma once

#include <common.h>

#include <SDL.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

class opencraft_video {
   public:
     opencraft_video(bool fullscreen, unsigned int w, unsigned int h);
     void init_video();
     void start_frame();
     void end_frame();
     void enter_2d();

   private:
     SDL_Window *screen;
     SDL_GLContext glcontext;
     bool is_fullscreen;
     unsigned int res_w;
     unsigned int res_h;
};
