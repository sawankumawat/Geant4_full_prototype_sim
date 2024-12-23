cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

#----------------------------------------------------------------------------
# Setup Section: You have to set the following two parameters
#----------------------------------------------------------------------------
# Set the name of your main program
  set( NAME_MAIN_PROGRAM  FoCal_Main)

# Set the path to Geant4 library
  set( Geant4_DIR /home/sawan/geant4/geant4-v11.1.2-install/lib/Geant4-11.1.2)
#----------------------------------------------------------------------------
# Intact Section: You don't need to touch the following part
#----------------------------------------------------------------------------
# Set the project name 
  project( ${NAME_MAIN_PROGRAM} ) 
#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})
#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED)

# ROOT version 6 required
if(ROOT_FOUND)
  STRING(REGEX MATCH "6.*" VERSION6MATCH ${ROOT_VERSION})
  if(NOT VERSION6MATCH)
    message(FATAL_ERROR "P01 requires ROOT 6")
  endif()
endif()

# Include ROOT's CMake functions for dictionary generation
include("${ROOT_DIR}/RootMacros.cmake")
#----------------------------------------------------------------------------
# Set up Geant4 include directories and compile definition
  include(${Geant4_USE_FILE})
  include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS})

# Locate sources and headers for this project
  file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
  file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Generate dictionaries, add ROOT libraries properties
#
add_library(${NAME_MAIN_PROGRAM}ClassesDict SHARED ${sources})
set(/home/mriganka/mystuff/lib/libexpat.so)
set(libsuffix .so)
set(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES} SUFFIX ${libsuffix})
set_target_properties(${NAME_MAIN_PROGRAM}ClassesDict PROPERTIES ${ROOT_LIBRARY_PROPERTIES})
target_link_libraries(${NAME_MAIN_PROGRAM}ClassesDict  ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
#----------------------------------------------------------------------------

# Compile and link your application
  add_executable(${NAME_MAIN_PROGRAM} ${NAME_MAIN_PROGRAM}.cc 
                                      ${sources} ${headers})
  target_link_libraries(${NAME_MAIN_PROGRAM} ${NAME_MAIN_PROGRAM}ClassesDict ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# Install the executable
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/..)
  install(TARGETS ${NAME_MAIN_PROGRAM} DESTINATION bin)
  install(TARGETS ${NAME_MAIN_PROGRAM}ClassesDict DESTINATION lib)
