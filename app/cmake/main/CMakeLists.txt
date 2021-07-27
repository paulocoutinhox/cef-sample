#
# These are included by the individual executable targets so the paths must be relative to those directories
#

#
# Source files
#

# Main executable sources
set(APP_SRCS
  ../../src/main/AppFactoryBrowser.cpp
  ../../include/main/AppBrowser.hpp
  ../../src/main/AppBrowser.cpp
  ../../src/main/AppClient.cpp
  ../../include/main/AppClient.hpp
  ../../src/main/RequestDumpResourceProvider.cpp
  ../../include/main/RequestDumpResourceProvider.hpp
  ../../src/main/binding/AppBinding.cpp
  ../../include/main/binding/AppBinding.hpp
  ../../src/main/net/RequestClient.cpp
  ../../include/main/net/RequestClient.hpp
  ../../src/main/main.cpp
  )

if(OS_LINUX OR OS_WINDOWS)
  # On Windows and Linux the same executable is used for all processes
  set(APP_SRCS ${APP_SRCS}
    ../../src/shared/AppFactoryEmptyOther.cpp
    ../../src/main/AppFactoryRenderer.cpp  
    ../../src/main/AppRenderer.cpp        
    ../../src/main/v8/AppExtension.cpp
    ../../include/main/v8/AppExtension.hpp    
    )
elseif(OS_MAC)
  # On macOS a separate helper executable is used for subprocesses
  set(APP_HELPER_SRCS
    ../../src/shared/AppFactoryEmptyOther.cpp
    ../../src/main/AppFactoryRenderer.cpp    
    ../../src/main/AppRenderer.cpp
    ../../src/main/macos/ProcessEntrypoint.cpp
    ../../src/main/v8/AppExtension.cpp
    ../../include/main/v8/AppExtension.hpp
    )
endif()

if(OS_MACOSX OR OS_WINDOWS)
  # On macOS and Windows include the shared resources
  set(APP_RESOURCES_SRCS ${SHARED_RESOURCES_SRCS} ${SHARED_ASSETS_DIR})
endif()

#
# Shared configuration
#

# Target executable names
set(APP_TARGET "MyApp")

if(OS_MAC)
  set(APP_HELPER_TARGET "MyApp_Helper")
  set(APP_HELPER_OUTPUT_NAME "MyApp Helper")
endif()

include_directories(../../include)

#
# Linux configuration
#

if(OS_LINUX)
  # Executable target
  add_executable(${APP_TARGET} ${APP_SRCS})
  set_app_executable_target_properties(${APP_TARGET})
  add_dependencies(${APP_TARGET} shared)
  target_link_libraries(${APP_TARGET} shared)

  # Manually copy assets directory
  add_custom_command(
      TARGET ${APP_TARGET}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_ASSETS_SRCS}"
        "${APP_TARGET_OUT_DIR}/assets"
      VERBATIM)
endif()

#
# Mac OS X configuration
#

if(OS_MAC)
  # Create source groups for Xcode
  set_app_source_groups("${APP_SRCS}")
  set_app_source_groups("${APP_HELPER_SRCS}")

  # Output path for the main app bundle
  set(APP_PATH "${APP_TARGET_OUT_DIR}/${APP_TARGET}.app")

  # Variables referenced from the main Info.plist file
  set(EXECUTABLE_NAME "${APP_TARGET}")
  set(PRODUCT_NAME "${APP_TARGET}")

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library
    add_logical_target("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  endif()

  # Main app bundle target
  add_executable(${APP_TARGET} MACOSX_BUNDLE ${APP_SRCS} ${APP_RESOURCES_SRCS})
  set_app_executable_target_properties(${APP_TARGET})
  add_dependencies(${APP_TARGET} shared)
  target_link_libraries(${APP_TARGET} shared)
  set_target_properties(${APP_TARGET} PROPERTIES RESOURCE "${APP_RESOURCES_SRCS}" MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_INFO_PLIST}")

  # Copy the CEF framework into the Frameworks directory
  add_custom_command(
    TARGET ${APP_TARGET}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy_directory
      "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
      "${APP_PATH}/Contents/Frameworks/Chromium Embedded Framework.framework"
    VERBATIM)

  # Create the multiple helper app bundle targets
  foreach(APP_HELPER_SUFFIX_LIST ${CEF_HELPER_APP_SUFFIXES}) 
    # Convert to a list and extract the suffix values
    string(REPLACE ":" ";" APP_HELPER_SUFFIX_LIST ${APP_HELPER_SUFFIX_LIST})

    list(GET APP_HELPER_SUFFIX_LIST 0 APP_HELPER_NAME_SUFFIX)
    list(GET APP_HELPER_SUFFIX_LIST 1 APP_HELPER_TARGET_NAME_SUFFIX)
    list(GET APP_HELPER_SUFFIX_LIST 2 APP_HELPER_PLIST_SUFFIX)

    # Define helper target and output names
    set(APP_HELPER_TARGET_NAME "${APP_HELPER_TARGET}${APP_HELPER_TARGET_NAME_SUFFIX}")
    set(APP_HELPER_TARGET_OUTPUT_NAME "${APP_HELPER_OUTPUT_NAME}${APP_HELPER_NAME_SUFFIX}")

    # Create helper specific variants of the helper-Info.plist file.
    # Do this manually because the configure_file command (which is executed as part of MACOSX_BUNDLE_INFO_PLIST) uses global env variables and would insert the wrong values with multiple targets.
    set(APP_HELPER_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${APP_HELPER_TARGET_NAME_SUFFIX}.plist")

    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_HELPER_INFO_PLIST}" APP_PLIST_CONTENTS)
    string(REPLACE "\${EXECUTABLE_NAME}" "${APP_HELPER_TARGET_OUTPUT_NAME}" APP_PLIST_CONTENTS ${APP_PLIST_CONTENTS})
    string(REPLACE "\${PRODUCT_NAME}" "${APP_HELPER_TARGET_OUTPUT_NAME}" APP_PLIST_CONTENTS ${APP_PLIST_CONTENTS})
    string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${APP_HELPER_PLIST_SUFFIX}" APP_PLIST_CONTENTS ${APP_PLIST_CONTENTS})
    file(WRITE ${APP_HELPER_INFO_PLIST} ${APP_PLIST_CONTENTS})

    # Create helper executable target
    add_executable(${APP_HELPER_TARGET_NAME} MACOSX_BUNDLE ${APP_HELPER_SRCS})
    set_app_executable_target_properties(${APP_HELPER_TARGET_NAME})
    add_dependencies(${APP_HELPER_TARGET_NAME} shared_helper)
    target_link_libraries(${APP_HELPER_TARGET_NAME} shared_helper)
    set_target_properties(${APP_HELPER_TARGET_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${APP_HELPER_INFO_PLIST} OUTPUT_NAME ${APP_HELPER_TARGET_OUTPUT_NAME})

    if(USE_SANDBOX)
      target_link_libraries(${APP_HELPER_TARGET_NAME} cef_sandbox_lib)
    endif()

    # Add the helper as a dependency of the main executable target
    add_dependencies(${APP_TARGET} "${APP_HELPER_TARGET_NAME}")

    # Copy the helper app bundle into the frameworks directory
    add_custom_command(
      TARGET ${APP_TARGET}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        "${APP_TARGET_OUT_DIR}/${APP_HELPER_TARGET_OUTPUT_NAME}.app"
        "${APP_PATH}/Contents/Frameworks/${APP_HELPER_TARGET_OUTPUT_NAME}.app"
      VERBATIM)
  endforeach()

  # Manually process and copy over resource files.
  # The Xcode generator can support this via the set_target_properties RESOURCE directive but that doesn't properly handle nested resource directories.
  # Remove these prefixes from input file paths.
  set(PREFIXES "../../resources/macos/")
  copy_mac_resources("${APP_RESOURCES_SRCS}" "${PREFIXES}" "${APP_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${APP_PATH}")

  # Manually copy assets directory
  add_custom_command(
      TARGET ${APP_TARGET}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_ASSETS_SRCS}"
        "${APP_PATH}/Contents/Resources/assets"
      VERBATIM)
endif()

#
# Windows configuration
#

if(OS_WINDOWS)
  # Add resources to the sources variable for convenience
  set(APP_SRCS ${APP_SRCS} ${APP_RESOURCES_SRCS})

  # Create source groups for Visual Studio
  set_app_source_groups("${APP_SRCS}")

  # Executable target
  add_executable(${APP_TARGET} WIN32 ${APP_SRCS})
  set_app_executable_target_properties(${APP_TARGET})
  add_dependencies(${APP_TARGET} shared)
  target_link_libraries(${APP_TARGET} shared)

  # Manually copy assets directory
  add_custom_command(
      TARGET ${APP_TARGET}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_ASSETS_SRCS}"
        "${APP_TARGET_OUT_DIR}/assets"
      VERBATIM)
endif()
