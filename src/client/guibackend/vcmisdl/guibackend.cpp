/*
 * License: GNU General Public License v2.0 or later
 */

#include "guibackend.h"

#include "Global.h"
#include "client/GameEngine.h"
#include "client/eventsSDL/InputHandler.h"

using namespace rev::guibackend;

void VcmiSdlGuiBackend::processEvents()
{
    ENGINE->input().fetchEvents();
}

void VcmiSdlGuiBackend::updateFrame()
{
    ENGINE->updateFrame();
}