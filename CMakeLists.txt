cmake_minimum_required(VERSION 3.0)

project(TRIDENTDP)

# Setup compiler
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)

if (COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else ()
    message (FATAL_ERROR "You need a compiler with standard c++11 support")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wall -Wextra ")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
# native optimized build
set(CMAKE_CXX_FLAGS_NATIVE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native"
        CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_NATIVE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native"
        CACHE STRING "" FORCE)
# profiling
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_NATIVE} -g -pg"
        CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_NATIVE} -g -pg"
        CACHE STRING "" FORCE)
# optimized debug
set(CMAKE_CXX_FLAGS_OPTDEBUG "${CMAKE_CXX_FLAGS_NATIVE} -g"
        CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_OPTDEBUG "${CMAKE_CXX_FLAGS_NATIVE} -g"
        CACHE STRING "" FORCE)


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

#----------------------------------------------------------------------------
#find the yaml-cpp package
find_package(yaml-cpp REQUIRED)

#----------------------------------------------------------------------------
FIND_PACKAGE(ROOT REQUIRED COMPONENTS Geom TMVA MathMore)
IF (ROOT_FOUND)
    INCLUDE_DIRECTORIES(${ROOT_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${ROOT_INCLUDE_DIR})
    LINK_LIBRARIES(${ROOT_LIBRARIES})
    LINK_DIRECTORIES(${ROOT_LIBRARY_DIR})
ENDIF ()

set(BS3_INSTALL_PATH $ENV{BS3_INSTALL_PATH})
IF(BS3_INSTALL_PATH)
    MESSAGE(STATUS "bamboo-shoot3 found at ${BS3_INSTALL_PATH}")
ELSE ()
    message(FATAL_ERROR "bamboo-shoot3 not found")
ENDIF ()


include(ExternalProject)

include_directories(
        include
        ${BS3_INSTALL_PATH}/include
        ${ROOT_INCLUDE_DIR}
)

link_libraries(${BS3_INSTALL_PATH}/lib/libpbsf.a ${ROOT_LIBRARIES})

# emulator
add_subdirectory(Emulator)

# data process chain
add_subdirectory(src)

