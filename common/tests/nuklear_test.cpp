//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 by Gareth Nelson (gareth@garethnelson.com)
//
// This file is part of OpenCraft.
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
//     Test of the nuklear GUI+app framework
//
//-----------------------------------------------------------------------------

#include <opencraft/common.h>
#include <opencraft/nuklear_gui.h>

using namespace opencraft;

#define APPNAME    "Nuklear test"
#define WIN_WIDTH  1280
#define WIN_HEIGHT 720

class test_app : public nuklear::app {
   public:
      test_app(std::string window_title, int win_width, int win_height) : app(window_title,win_width,win_height) {}
      void draw_gui() {
           int WIN_FLAGS = NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE;
           if (nk_begin(ctx, "Nuklear test", nk_rect(200, 200, 210, 250),WIN_FLAGS)) {
           }
           nk_end(ctx);
     }
};

int main(int argc, char** argv) {
    init_common(APPNAME);

    test_app TestApp(APPNAME,WIN_WIDTH,WIN_HEIGHT);
    TestApp.init();
    for(;;) TestApp.iter_loop();

}

