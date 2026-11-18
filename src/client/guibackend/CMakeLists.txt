add_library(guibackend STATIC)

target_sources(guibackend PRIVATE
    iguibackend.h
    vcmisdl/guibackend.h
    vcmisdl/guibackend.cpp
    vcmisdl/GameEngine.cpp
    vcmisdl/GameEngine.h
)

target_include_directories(guibackend PRIVATE 
    ${VCMI_SOURCE_DIR}/client
)

target_link_libraries(guibackend PRIVATE
	vcmiclientstatic
	rev_geometry
	SDL2::SDL2 SDL2::Image SDL2::Mixer SDL2::TTF
)