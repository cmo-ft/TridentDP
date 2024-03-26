#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(include)

#----------------------------------------------------------------------------
# Find required packages)
#

include_directories(../include)
file(GLOB  Emulator_sources src/* ../src/data_structure/Event.cc)
file(GLOB  Emulator_headers include/*)

#----------------------------------------------------------------------------
# Add the executable, and link it to the ROOT libraries
#
add_executable(Emulator Emulator.cpp ${Emulator_sources} ${Emulator_headers})
target_link_libraries(Emulator ${YAML_CPP_LIBRARIES})


install(TARGETS Emulator DESTINATION ./)