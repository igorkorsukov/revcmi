add_library(guibackend STATIC)

target_sources(guibackend PRIVATE
    iguibackend.h
    vcmisdl/guibackend.h
    vcmisdl/guibackend.cpp
)

target_link_libraries(guibackend PRIVATE vcmiclientstatic)

