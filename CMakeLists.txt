cmake_minimum_required (VERSION 3.5.1)
project (ComgrUtils)

set (COMGR_UTILS_VERSION_MAJOR 1)
set (COMGR_UTILS_VERSION_MINOR 0)

# Path to the comgr header
include_directories(AFTER ${COMGR_INCLUDE_DIR} )

# Add all header and source files within the directory to the library.
file (GLOB CPP_INC
    "Src/ComgrUtils.h"
)

# Add all source files found within this directory.
file (GLOB CPP_SRC
    "Src/ComgrUtils.cpp"
)

# Pick up the source files that are relevant to the platform
add_library(${PROJECT_NAME} STATIC ${CPP_SRC} ${CPP_INC})

# Added since ComgrUtils is included in a dynamic object (RgpFileAnalyzer)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

