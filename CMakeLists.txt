cmake_minimum_required(VERSION 3.16)
project(NuBeam LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(GNUInstallDirs)
# Keep RPATH in installed binaries and derive it automatically from link dirs
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

option(WITH_GEANT4_UIVIS "Enable Geant4 UI and visualization drivers" ON)

# --- Dependencies -------------------------------------------------------------
# Geant4 10.4 uses the old style (variables + Geant4_USE_FILE). Try components;
# fall back gracefully if targets aren’t exported.
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 10.4 REQUIRED COMPONENTS gdml ui_all vis_all)
else()
  find_package(Geant4 10.4 REQUIRED COMPONENTS gdml)
endif()

# If the old helper is provided, include it (sets include dirs/defs/flags).
if(DEFINED Geant4_USE_FILE)
  include(${Geant4_USE_FILE})
endif()

find_package(ROOT 6.00 CONFIG REQUIRED COMPONENTS Core RIO Tree Hist)

# dk2nu finder lives here
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(DK2NU REQUIRED)

# Warnings
if(MSVC)
  add_compile_options(/permissive- /W3)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src)
add_subdirectory(scripts)

