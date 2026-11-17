
add_executable(revclient main.cpp)

target_link_libraries(revclient PRIVATE vcmiclientstatic)
target_link_libraries(revclient PRIVATE guibackend)

target_include_directories(revclient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

vcmi_set_output_dir(revclient "")

install(TARGETS revclient DESTINATION ${BIN_DIR})

