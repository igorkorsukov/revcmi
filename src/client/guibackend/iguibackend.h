/*
 * License: GNU General Public License v2.0 or later
 */
#pragma once

namespace rev::guibackend {
class IGuiBackend 
{
public:

    virtual ~IGuiBackend() = default;

    virtual void processEvents() = 0;
    virtual void updateFrame() = 0;

};
}