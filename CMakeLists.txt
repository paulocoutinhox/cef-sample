cmake_minimum_required(VERSION 3.13)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

#
# Project configuration
#

project("MyApp")

# CEF configuration
set_property(GLOBAL PROPERTY OS_FOLDERS ON)
set(CEF_VERSION "138.0.2+g1f564be+chromium-138.0.7204.15")

# Determine the platform
if(NOT DEFINED CEF_PLATFORM)
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    if("${PROJECT_ARCH}" STREQUAL "arm64")
      set(CEF_PLATFORM "macosarm64")
    else()
      set(CEF_PLATFORM "macosx64")
    endif()
  elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    if(CMAKE_SIZEOF_VOID_P MATCHES 8)
      set(CEF_PLATFORM "linux64")
    else()
      set(CEF_PLATFORM "linux32")
    endif()
  elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P MATCHES 8)
      set(CEF_PLATFORM "windows64")
    else()
      set(CEF_PLATFORM "windows32")
    endif()
  endif()
endif()

# Add this project's cmake/ directory to the module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/app/cmake/cef")

# Download and extract the CEF binary distribution (executes DownloadCEF.cmake)
include(DownloadCEF)
downloadcef("${CEF_PLATFORM}" "${CEF_VERSION}" "${CMAKE_SOURCE_DIR}/temp/cef")

# Add the CEF binary distribution's cmake/ directory to the module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Load the CEF configuration (executes FindCEF.cmake)
find_package(CEF REQUIRED)

#
# Target configuration
#

# Include the libcef_dll_wrapper target (executes libcef_dll/CMakeLists.txt)
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Allow includes relative to the current source directory
include(${CMAKE_CURRENT_SOURCE_DIR}/app/cmake/shared/common.cmake)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app/cmake/shared)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/app/cmake/main)

# Display configuration settings
print_cef_config()
