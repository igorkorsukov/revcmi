
add_executable(revclient main.cpp)

target_link_libraries(revclient PRIVATE vcmiclientstatic)

vcmi_set_output_dir(revclient "")

install(TARGETS revclient DESTINATION ${BIN_DIR})

