#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(include)

#----------------------------------------------------------------------------
# Find required packages)
#
find_package(yaml-cpp REQUIRED)
#----------------------------------------------------------------------------
FIND_PACKAGE(ROOT REQUIRED COMPONENTS Geom TMVA MathMore)
IF (ROOT_FOUND)
    INCLUDE_DIRECTORIES(${ROOT_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${ROOT_INCLUDE_DIR})
    LINK_LIBRARIES(${ROOT_LIBRARIES})
    LINK_DIRECTORIES(${ROOT_LIBRARY_DIR})
ENDIF ()

include_directories(../include)
file(GLOB  Emulator_sources src/* ../src/data_structure/Event.cc)
file(GLOB  Emulator_headers include/*)

#----------------------------------------------------------------------------
# Add the executable, and link it to the ROOT libraries
#
add_executable(Emulator Emulator.cpp ${Emulator_sources} ${Emulator_headers})
target_link_libraries(Emulator ${YAML_CPP_LIBRARIES})


install(TARGETS Emulator DESTINATION ./)
