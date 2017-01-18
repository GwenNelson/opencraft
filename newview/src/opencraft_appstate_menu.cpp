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
//     Menu appstate
//
//-----------------------------------------------------------------------------

#include <common.h>
#include <opencraft_appstate_menu.h>
#include <opencraft_video.h>

#include <SOIL.h>
#include <physfs.h>
#include <string>

extern opencraft_video *oc_video;
extern std::string title_png_path;

opencraft_appstate_menu::opencraft_appstate_menu() {
    oc_video->enter_2d();
    this->title_gl_tex_id = 0;
    PHYSFS_File* fd = PHYSFS_openRead(title_png_path.c_str());
    
    if(fd==NULL) {
       LOG(error) << "Could not load title PNG!";
    }

    PHYSFS_uint32 f_size = PHYSFS_fileLength(fd);
    void*         f_data = malloc(f_size);
    PHYSFS_read(fd,f_data,(PHYSFS_uint32)f_size,1);
    PHYSFS_close(fd);
    this->title_gl_tex_id = SOIL_load_OGL_texture_from_memory(
                            (const unsigned char*)f_data,
                            f_size,
                            SOIL_LOAD_AUTO,
                            SOIL_CREATE_NEW_ID,0);
    free(f_data);
}

void opencraft_appstate_menu::update_state(SDL_Event *ev) {
}

void opencraft_appstate_menu::render() {
     glEnable(GL_BLEND);
     glBindTexture(GL_TEXTURE_2D, this->title_gl_tex_id);
     glColor4f(1.0f,1.0f,1.0f,1.0f);
     float x = 10.0f;
     float y = 10.0f;
     float w = 512.0f;
     float h = 128.0f;
     glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y );
            glTexCoord2f(1.0f, 0.0f); glVertex2f(x+w, y );
            glTexCoord2f(1.0f, 1.0f); glVertex2f(x+w,  y+h );
            glTexCoord2f(0.0f, 1.0f); glVertex2f(x,  y+h );
     glEnd();
     glDisable(GL_BLEND);
}

