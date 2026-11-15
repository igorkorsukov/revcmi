cmake_minimum_required(VERSION 3.21)

project(discord-rpc)

option(DISCORD_RPC_NO_GLAZE "Use minimal built-in JSON serializer instead of glaze (C++17 compatible)" OFF)

if (DISCORD_RPC_NO_GLAZE)
  set(CMAKE_CXX_STANDARD 17)
else()
  set(CMAKE_CXX_STANDARD 23)
endif()

add_library(${PROJECT_NAME} STATIC src/discord-rpc.cpp src/serialization.cpp src/command-queue.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Windows has some extra code
if (WIN32)
  target_sources(${PROJECT_NAME} PRIVATE src/platform/windows.cpp)
  target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32.lib)
endif()

if (NOT COMMAND CPMAddPackage)
  include(cmake/CPM.cmake)
endif()

if (NOT TARGET fmt::fmt)
  CPMAddPackage("gh:fmtlib/fmt#12.1.0")
endif()

if (NOT DISCORD_RPC_NO_GLAZE AND NOT TARGET glaze::glaze)
  CPMAddPackage("gh:stephenberry/glaze@7.0.2")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

if (NOT DISCORD_RPC_NO_GLAZE)
  target_link_libraries(${PROJECT_NAME} PRIVATE glaze::glaze)
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE DISCORD_RPC_NO_GLAZE)
endif()

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(DISCORD_RPC_BUILD_TESTS ON)
endif()

if (DISCORD_RPC_BUILD_TESTS)
  add_subdirectory(test)
endif()
