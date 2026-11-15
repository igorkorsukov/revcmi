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

/// Entry point of the VCMI client.
/// Contains the whole initialization sequence of the client originally implemented
/// directly in main(). The process entry point itself lives in a separate executable
/// (see src/client/main.cpp), which only forwards its arguments here.
///
/// Returns the process exit code.
int vcmiclientMain(int argc, char * argv[]);
