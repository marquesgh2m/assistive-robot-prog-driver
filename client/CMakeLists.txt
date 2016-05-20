CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
PROJECT (donnie_client)

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")

INCLUDE (UsePlayerC++)

PLAYER_ADD_PLAYERCPP_CLIENT (donnie_client SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/donnie_client.cc)
