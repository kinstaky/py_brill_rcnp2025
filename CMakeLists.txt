# Require the cmake minimum version.
cmake_minimum_required(VERSION 3.16)

if (NOT SKBUILD)
  message(WARNING "\
  This CMake file is meant to be executed using 'scikit-build'. Running
  it directly will almost certainly not produce the desired result. If
  you are a user trying to install this package, please use the command
  below, which will install all necessary build dependencies, compile
  the package in an isolated environment, and then install it.
  =====================================================================
   $ pip install .
  =====================================================================
  If you are a software developer, and this is your own package, then
  it is usually much more efficient to install the build dependencies
  in your environment once and use the following command that avoids
  a costly creation of a new virtual environment at every compilation:
  =====================================================================
   $ pip install nanobind scikit-build-core[pyproject]
   $ pip install --no-build-isolation -ve .
  =====================================================================
  You may optionally add -Ceditable.rebuild=true to auto-rebuild when
  the package is imported. Otherwise, you need to re-run the above
  after editing C++ files.")
endif()

# Set project properties.
project(
	${SKBUILD_PROJECT_NAME}
	VERSION ${SKBUILD_PROJECT_VERSION}
	DESCRIPTION "brill_rcnp2025 python bindings"
	LANGUAGES CXX
)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
  cmake_policy(SET CMP0135 NEW)
endif()

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if (EXISTS "${LOC_PATH}")
	message(FATAL_ERROR "You cannot build in a source directory (or any directory with"
						"CmakeLists.txt file). Please make a build subdirectory. Feel free"
						"to remove CMakeCache.txt and CMakeFiles.")
endif()

# Set the build type
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()


# Only do these if this is the main project, and not if it is included
# through add_subdirectory
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

	# Let's ensure -std=c++xx instead of -std=g++xx
	set(CMAKE_CXX_EXTENSIONS OFF)

	# Set the default cxx standard
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)

	# Set default compile options
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra")

	# Let's nicely support folders in IDE's
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Try to import all Python components potentially needed by nanobind
# find python
if (CMAKE_VERSION VERSION_LESS 3.18)
  set(DEV_MODULE Development)
else()
  set(DEV_MODULE Development.Module)
endif()
find_package(Python 3.9
  REQUIRED COMPONENTS Interpreter ${DEV_MODULE}
  OPTIONAL_COMPONENTS Development.SABIModule)

# Import nanobind through CMake's find_package mechanism
find_package(nanobind CONFIG REQUIRED)
# cern ROOT
find_package(ROOT 6.20 CONFIG REQUIRED COMPONENTS Hist)

include(FetchContent)
# Keep project libraries inside the Python extension instead of requiring
# separate shared libraries at import time.
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Import brill_rcnp2025
FetchContent_Declare(
	brill_rcnp2025
	GIT_REPOSITORY https://github.com/kinstaky/brill_rcnp2025.git
	GIT_TAG master
)
FetchContent_MakeAvailable(brill_rcnp2025)

# Option to enable tests (default OFF for development)
# option(BUILD_TESTS "Build unit tests" OFF)
# option(BUILD_BIN "Build executable binary" OFF)

function(show_config_options)
	message("")
	message("Build Properties")
	message("  c++ compiler.................: " ${CMAKE_CXX_COMPILER})
	message("  c++ stardard.................: " ${CMAKE_CXX_STANDARD})
	message("  python interpreter...........: " ${Python_INTERPRETER_ID})
	message("  python version...............: " ${Python_VERSION})
	message("  python include dirs..........: " ${Python_INCLUDE_DIRS})
	message("  cmake........................: " ${CMAKE_VERSION})
	message("  cmake build type.............: " ${CMAKE_BUILD_TYPE})
	# message("Build Options")
	# message("  Build tests..................: " ${BUILD_TESTS})
	# message("  Build binary.................: " ${BUILD_BIN})
	# message("")
endfunction()

show_config_options()

add_subdirectory(cpp)
add_subdirectory(bindings)
# if (BUILD_BIN)
# 	add_subdirectory(bin)
# endif()
# if(BUILD_TESTS)
# 	enable_testing()
# 	add_subdirectory(tests)
# endif()
