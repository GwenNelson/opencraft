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

static int power_of_two(int input)
{
    int value = 1;

    while ( value < input ) {
        value <<= 1;
    }
    return value;
}

GLuint SDL_GL_LoadTexture(SDL_Surface *surface)
{
    GLfloat texcoord[4];
    GLuint texture;
    int w, h;
    SDL_Surface *image;
    SDL_Rect area;
    Uint8  saved_alpha;
    SDL_BlendMode saved_mode;

    /* Use the surface width and height expanded to powers of 2 */
//    w = power_of_two(surface->w);
//    h = power_of_two(surface->h);
    w = surface->w;
    h = surface->h;
    texcoord[0] = 0.0f;         /* Min X */
    texcoord[1] = 0.0f;         /* Min Y */
    texcoord[2] = (GLfloat)surface->w / w;  /* Max X */
    texcoord[3] = (GLfloat)surface->h / h;  /* Max Y */

    image = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            w, h,
            32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
            0x000000FF,
            0x0000FF00,
            0x00FF0000,
            0xFF000000
#else
            0xFF000000,
            0x00FF0000,
            0x0000FF00,
            0x000000FF
#endif
               );
    if ( image == NULL ) {
        return 0;
    }

    /* Save the alpha blending attributes */
    SDL_GetSurfaceAlphaMod(surface, &saved_alpha);
    SDL_SetSurfaceAlphaMod(surface, 0xFF);
    SDL_GetSurfaceBlendMode(surface, &saved_mode);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

    /* Copy the surface into the GL texture image */
    area.x = 0;
    area.y = 0;
    area.w = surface->w;
    area.h = surface->h;
    SDL_BlitSurface(surface, &area, image, &area);

    /* Restore the alpha blending attributes */
    SDL_SetSurfaceAlphaMod(surface, saved_alpha);
    SDL_SetSurfaceBlendMode(surface, saved_mode);

    /* Create an OpenGL texture for the image */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGBA,
             w, h,
             0,
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             image->pixels);
    SDL_FreeSurface(image); /* No longer needed */

    return texture;
}

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

void predraw_text(void* font, int r, int g, int b, char* text, int *w, int *h, int *text_w, int *text_h, GLuint *tex_out) {
     TTF_Font *sdl_font = (TTF_Font*)font;
     SDL_Surface *sdl_output;
     SDL_Color font_col = {r,g,b,0};
     sdl_output = TTF_RenderText_Solid(sdl_font,(const char*)text,font_col);
     *tex_out = SDL_GL_LoadTexture(sdl_output);
     *w = sdl_output->w;
     *h = sdl_output->h;

     TTF_SizeText(sdl_font,(const char*)text,text_w,text_h);
     SDL_FreeSurface(sdl_output);
}

void draw_transparent_quad(float x, float y, float w, float h, GLuint tex_id) {
     glEnable(GL_BLEND);
     draw_textured_quad(x,y,w,h,(unsigned int)tex_id);
     glDisable(GL_BLEND);
}

void draw_text(float x, float y, void* font, int r, int g, int b, char* text) {
     int w,h;
     int text_w,text_h;
     GLuint text_tex;
     predraw_text(font,r,g,b,text,&w,&h,&text_w,&text_h,&text_tex);
     draw_transparent_quad(x,y,w,h,text_tex);
}



