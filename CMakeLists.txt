cmake_minimum_required(VERSION 3.16.0)
project(revcmi)

# VCMI sources live in the "vcmi" subdirectory of this repository.
# Everything inside vcmi/ refers to that directory via VCMI_SOURCE_DIR instead of
# CMAKE_SOURCE_DIR/CMAKE_HOME_DIRECTORY/PROJECT_SOURCE_DIR, because those point to
# the top-level directory of the whole build tree.
set(VCMI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcmi")

add_subdirectory(vcmi)
