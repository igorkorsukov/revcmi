/*
 * EntryPoint.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include <cstdint>

enum class InitCode : std::uint8_t {
    Quit,
    Continue,
};

InitCode init_vcmi(int argc, char * argv[]);
void run_vcmi();
void deinit_vcmi();
