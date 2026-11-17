/*
 * License: GNU General Public License v2.0 or later
 */
#pragma once

#include "../iguibackend.h"

namespace rev::guibackend {
class VcmiSdlGuiBackend : public IGuiBackend
{
public:
    VcmiSdlGuiBackend() = default;

    void processEvents() override;
    void updateFrame() override;
};
}