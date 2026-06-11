#######################################################################################################################
### Copyright (c) 2025 Advanced Micro Devices, Inc. All rights reserved.
### @author AMD Developer Tools Team
### @brief Define and fetch external dependencies for ComgrUtils.
#######################################################################################################################

include(FetchContent)

########################################
# Define release name and build number.
########################################

set(LLVM_BUILD_NUM "93")
set(RDTS_RELEASE "rdts-2025-11-03")

####################
# Put together URL.
####################

# OS-dependent URL bits.
if(WIN32)
    # Suffix used for package archive files.
    set(ARCHIVE_SUFFIX ".zip")
    # OS name used at the end of the COMGR package archive filename.
    set(COMGR_OS_SUFFIX "windows")
elseif(LINUX)
    # Suffix used for package archive files.
    set(ARCHIVE_SUFFIX ".tar.gz")
    # OS name used at the end of the COMGR package archive filename.
    set(COMGR_OS_SUFFIX "linux")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Assemble the full URL.
set(GPUOPEN_GITHUB_COMGR_RELEASE "https://github.com/GPUOpen-Tools/comgr_utils/releases/download")
string(CONCAT COMGR_URL
	${GPUOPEN_GITHUB_COMGR_RELEASE} / ${RDTS_RELEASE} /
	"COMGR_" ${COMGR_OS_SUFFIX} "_" ${LLVM_BUILD_NUM} ${ARCHIVE_SUFFIX})

#####################################
# Download comgr and set up targets.
#####################################
FetchContent_Declare(comgr
    URL ${COMGR_URL}
    SOURCE_DIR "${PROJECT_SOURCE_DIR}/external/comgr")

FetchContent_MakeAvailable(comgr)

# Define imported targets for pre-built comgr library.
# TODO we can remove this if we package comgr/LLVM with CPack and use exported targets from the package.
# Define files that may be required by the imported comgr targets.
if (WIN32)
    set(COMGR_SHARED_LIB "${comgr_SOURCE_DIR}/lib/VS2022/x64/amd_comgr_3.dll")
    set(COMGR_IMPLIB "${comgr_SOURCE_DIR}/lib/VS2022/x64/amd_comgr_3.lib")
elseif (LINUX)
    set(COMGR_SHARED_LIB "${comgr_SOURCE_DIR}/lib/x64/libamd_comgr.so.3.0.0")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()
set (COMGR_INCLUDE_DIR "${comgr_SOURCE_DIR}/include")

# Check whether required files exist.
if (NOT EXISTS ${COMGR_SHARED_LIB})
   message(FATAL_ERROR "comgr shared library ${COMGR_SHARED_LIB} does not exist!")
endif()
if (NOT EXISTS ${COMGR_INCLUDE_DIR})
   message(FATAL_ERROR "comgr include directory ${COMGR_INCLUDE_DIR} does not exist!")
endif()

# Define the imported targets.
if (COMGR_UTILS_DYNAMIC_LINKING)
    add_library(comgr INTERFACE IMPORTED GLOBAL)
    target_include_directories(comgr INTERFACE ${COMGR_INCLUDE_DIR})
    
    add_library(comgr_plugin MODULE IMPORTED GLOBAL)
    set_target_properties(comgr_plugin PROPERTIES IMPORTED_LOCATION ${COMGR_SHARED_LIB})
else()
    add_library(comgr SHARED IMPORTED GLOBAL)
    set_target_properties(comgr PROPERTIES IMPORTED_LOCATION ${COMGR_SHARED_LIB})
    target_include_directories(comgr INTERFACE ${COMGR_INCLUDE_DIR})
    # When defining comgr as a shared library on Windows, we additionally need the static import library.
    # Check to ensure it exists as well.
    if (WIN32)
        if (NOT EXISTS ${COMGR_IMPLIB})
	    message(FATAL_ERROR "comgr import library ${COMGR_IMPLIB} does not exist!")
        endif()
        set_target_properties(comgr PROPERTIES IMPORTED_IMPLIB ${COMGR_IMPLIB})
    elseif (LINUX)
        set_target_properties(comgr PROPERTIES IMPORTED_SONAME "libamd_comgr.so.3")
    endif()
endif()
