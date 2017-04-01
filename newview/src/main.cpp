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
//     Main entry point for the client
//
//-----------------------------------------------------------------------------

#include <opencraft/appfw/appfw.h>
#include <opencraft/appfw/interfaces/cli/simple_cli.h>
#include <opencraft/appfw/appstate/fsm/base_state.h>

#include <opencraft/appfw/appstate/fsm/base_fsm.h>

using namespace opencraft;

appfw::App *ClientApp;

int main(int argc, char** argv) {
    ClientApp = new appfw::App();
    ClientApp->AddInterface(new appfw::interfaces::cli::BaseCLI(ClientApp));
    ClientApp->run();
}

