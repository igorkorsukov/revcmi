/*
 * License: GNU General Public License v2.0 or later
 */

#include <iostream>
#include <memory>

#include "clientapp/EntryPoint.h"

#include "guibackend/vcmisdl/guibackend.h"

int main(int argc, char * argv[])
{
	InitCode init_code = init_vcmi(argc, argv);
	if (init_code == InitCode::Quit) {
		return 0;
	}

	auto guiBackend = std::make_unique<rev::guibackend::VcmiSdlGuiBackend>();

	try {
		while (true) {
			guiBackend->processEvents();
			guiBackend->updateFrame();
		}
	} catch (const std::exception & e) {
		std::cout << "Main loop termination requested: " << e.what() << std::endl;
	}


	deinit_vcmi();
	return 0;
}
