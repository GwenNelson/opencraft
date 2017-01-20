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
//     Rendering stuff for 2D rendering
//
//-----------------------------------------------------------------------------

#pragma once

#include <common.h>
#include <opencraft_video.h>

#include <SOIL.h>
#include <physfs.h>
#include <string>

#include <SDL.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

unsigned int load_texture(std::string filename);
void draw_textured_quad(float x, float y, float w, float h, unsigned int tex_id);

void draw_tiled_quad(float x, float y, float w, float h, float tile_w, float tile_h, unsigned int tex_id);

void* load_font(std::string filename,unsigned int size);
