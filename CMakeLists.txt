#######################################################################################################################
### Copyright (c) 2018-2026 Advanced Micro Devices, Inc. All rights reserved.
### @author AMD Developer Tools Team
#######################################################################################################################

cmake_minimum_required (VERSION 3.23)
project (ComgrUtils)

set (COMGR_UTILS_VERSION_MAJOR 1)
set (COMGR_UTILS_VERSION_MINOR 0)

option(COMGR_UTILS_DYNAMIC_LINKING
"If OFF, comgr will be imported as a SHARED library which ComgrUtils will link against.
If ON, comgr will be imported as a MODULE library and an INTERFACE library
and ComgrUtils will load it at runtime."
    OFF)

include("${PROJECT_SOURCE_DIR}/cmake/comgr_utils_external.cmake")

# Add all header and source files within the directory to the library.
file (GLOB CPP_INC
    "source/comgr_utils.h"
)

# Add all source files found within this directory.
file (GLOB CPP_SRC
    "source/comgr_utils.cpp"
)

add_library(${PROJECT_NAME} STATIC)
add_library(AMD::ComgrUtils ALIAS ${PROJECT_NAME})

# Pick up the source files that are relevant to the platform
target_sources(${PROJECT_NAME} PRIVATE ${CPP_SRC})
target_sources(${PROJECT_NAME} PUBLIC
               FILE_SET HEADERS
               BASE_DIRS "source"
               FILES ${CPP_INC})

target_link_libraries(${PROJECT_NAME} PUBLIC comgr)

if (COMGR_UTILS_DYNAMIC_LINKING)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC COMGR_DYNAMIC_LINKING)
endif()

# Added since comgr_utils is included in a dynamic object (RgpFileAnalyzer)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
