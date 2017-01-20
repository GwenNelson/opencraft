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

#include <common.h>
#include <opencraft_video.h>

#include <SOIL.h>
#include <physfs.h>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

extern opencraft_video *oc_video;
extern std::string texturepack_path;

unsigned int load_texture(std::string filename) {
    std::string fullpath = texturepack_path + filename;
    LOG(debug) << "Loading texture " << fullpath;
    unsigned int retval = -1;
    PHYSFS_File* fd = PHYSFS_openRead(fullpath.c_str());
    
    if(fd==NULL) {
       return -1;
    }

    PHYSFS_uint32 f_size = PHYSFS_fileLength(fd);
    void*         f_data = malloc(f_size);
    PHYSFS_read(fd,f_data,(PHYSFS_uint32)f_size,1);
    PHYSFS_close(fd);
    retval = SOIL_load_OGL_texture_from_memory(
             (const unsigned char*)f_data,
             f_size,
             SOIL_LOAD_AUTO,
             SOIL_CREATE_NEW_ID,0);
    free(f_data);
    return retval;
}

void draw_textured_quad(float x, float y, float w, float h, unsigned int tex_id) {
     glBindTexture(GL_TEXTURE_2D, tex_id);
     glColor4f(1.0f,1.0f,1.0f,1.0f);
     glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y );
            glTexCoord2f(1.0f, 0.0f); glVertex2f(x+w, y );
            glTexCoord2f(1.0f, 1.0f); glVertex2f(x+w,  y+h );
            glTexCoord2f(0.0f, 1.0f); glVertex2f(x,  y+h );
     glEnd();
}

void draw_tiled_quad(float x, float y, float w, float h, float tile_w, float tile_h, unsigned int tex_id) {
     glBindTexture(GL_TEXTURE_2D, tex_id);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     draw_textured_quad(x,y,w,h,tex_id);
     glColor4f(1.0f,1.0f,1.0f,1.0f);
     glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y );
            glTexCoord2f(w/tile_w, 0.0f); glVertex2f(x+w, y );
            glTexCoord2f(w/tile_w, h/tile_h); glVertex2f(x+w,  y+h );
            glTexCoord2f(0.0f, h/tile_h); glVertex2f(x,  y+h );
     glEnd();
}

void* load_font(std::string filename,unsigned int size) {
      if(!TTF_WasInit()) {
         TTF_Init();
      }
      LOG(debug) << "Loading font " << filename;

      std::string fullfontpath = "/fonts/" + filename;

      PHYSFS_file *fd     = PHYSFS_openRead((const char*)fullfontpath.c_str());
      unsigned int f_size = PHYSFS_fileLength(fd);
      void* buf           = malloc(f_size);
      PHYSFS_read(fd,buf,(PHYSFS_uint32)f_size,1);
      PHYSFS_close(fd);

      TTF_Font *font = TTF_OpenFontRW(SDL_RWFromMem(buf,f_size),1,size);
      LOG(debug) << "Loaded font!";
      return (void*)font;
}






