cmake_minimum_required(VERSION 3.10)
project(atomspace_server CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Only list install files that have actually changed.
set(CMAKE_INSTALL_MESSAGE "LAZY")

IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -std=libc++17")
ENDIF()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH 1)
endif()
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(BOOST_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.46 COMPONENTS system filesystem program_options)

IF(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
ELSE(Boost_FOUND)
    message(FATAL_ERROR "Boost libraries not found. Please install them")
ENDIF(Boost_FOUND)

#cogutil
find_package(CogUtil 3.10 CONFIG REQUIRED)
IF (COGUTIL_FOUND)
    message(STATUS "CogUtil version ${COGUTIL_VERSION} found.")
    add_definitions(-DHAVE_COGUTIL)
    set(HAVE_COGUTIL 1)
ELSE (COGUTIL_FOUND)
    message(FATAL_ERROR "CogUtil missing: it is needed!")
ENDIF (COGUTIL_FOUND)

# add the 'cmake' directory from cogutil to search path
list(APPEND CMAKE_MODULE_PATH  ${COGUTIL_DATA_DIR}/cmake)

#atomspace
find_package(AtomSpace 5.0.3 CONFIG REQUIRED)
IF(ATOMSPACE_FOUND)
    message(STATUS "AtomSpace found.")
    add_definitions(-DHAVE_ATOMSPACE)
    set(HAVE_ATOMSPACE 1)
ELSE (ATOMSPACE_FOUND)
    message(FATAL_ERROR "AtomSpace missing: it is needed!")
ENDIF (ATOMSPACE_FOUND)

include(OpenCogFindGuile)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(src)

# Needed for unit tests.

find_package(Cxxtest)
IF (CXXTEST_FOUND)
    message(STATUS "CxxTest found.")
    add_custom_target(tests)
    add_subdirectory(tests EXCLUDE_FROM_ALL)
ELSE (CXXTEST_FOUND)
    message(STATUS "CxxTest missing: needed for unit tests.")
ENDIF (CXXTEST_FOUND)
