if(OS_MAC)
  # Shared macOS resources (menus, icons, etc)
  set(SHARED_RESOURCES_SRCS ../../resources/macos/English.lproj/InfoPlist.strings ../../resources/macos/English.lproj/MainMenu.xib ../../resources/macos/Info.plist ../../resources/macos/app.icns)

  # Info.plist files used for main and helper app bundles
  set(SHARED_INFO_PLIST ../../resources/macos/Info.plist)
  set(SHARED_HELPER_INFO_PLIST ../../resources/macos/helper-Info.plist)
elseif(OS_WINDOWS)
  # Shared Windows resources (version information, icons)
  set(SHARED_RESOURCES_SRCS ../../resources/windows/big.ico ../../resources/windows/resource.h ../../resources/windows/app.rc ../../resources/windows/small.ico)

  # Manifest files used for executables
  set(SHARED_EXE_MANIFEST ../../resources/windows/app.exe.manifest)
  set(SHARED_COMPATIBILITY_MANIFEST ../../resources/windows/compatibility.manifest)
endif()

set(SHARED_ASSETS_SRCS ../../assets)

#
# Shared configuration
#

# Support nice project/target organization in Visual Studio and Xcode
if(OS_MACOSX OR OS_WINDOWS)
  # Enable the creation of project folders
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Group target contents by directory
  macro(set_app_source_groups srcs)
    foreach(FILE ${srcs})
      # Get the absolute directory path
      get_filename_component(ABS_FILE "${FILE}" ABSOLUTE)
      get_filename_component(PARENT_DIR "${ABS_FILE}" DIRECTORY)

      # Remove the common directory prefix, what remains in the group
      string(REPLACE "${CMAKE_SOURCE_DIR}/app/" "" GROUP "${PARENT_DIR}")

      # Convert to Windows slashes
      string(REPLACE "/" "\\" GROUP "${GROUP}")

      source_group("${GROUP}" FILES "${FILE}")
    endforeach()
  endmacro()
endif()

# Set the configuration-specific binary output directory
if(GEN_NINJA OR GEN_MAKEFILES)
  # Force Ninja and Make to create a subdirectory named after the configuration
  set(APP_TARGET_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
else()
  set(APP_TARGET_OUT_DIR "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
endif()

# Set properties common to all example targets
macro(set_app_properties target)
  # Output all binaries to the configuration-specific build directory
  set_target_properties(
    ${target}
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${APP_TARGET_OUT_DIR}"
               RUNTIME_OUTPUT_DIRECTORY "${APP_TARGET_OUT_DIR}"
               LIBRARY_OUTPUT_DIRECTORY "${APP_TARGET_OUT_DIR}")

  if(OS_MACOSX OR OS_WINDOWS)
    # Place the target in the "app" folder in Visual Studio and Xcode
    set_property(TARGET ${target} PROPERTY FOLDER "app")
  endif()
endmacro()

# Set properties on an example library target
macro(set_app_library_target_properties target)
  set_library_target_properties(${target})
  set_app_properties(${target})
endmacro()

# Set properties on an example executable target
macro(set_app_executable_target_properties target)
  set_executable_target_properties(${target})
  set_app_properties(${target})

  if(OS_LINUX)
    # Set rpath so that libraries can be placed next to the executable
    set_target_properties(${target} PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(${target} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
  elseif(OS_WINDOWS)
    # Add the custom Windows manifest files to the executable
    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMAND
        "mt.exe" -nologo -manifest
        \"${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_EXE_MANIFEST}\"
        \"${CMAKE_CURRENT_SOURCE_DIR}/${SHARED_COMPATIBILITY_MANIFEST}\"
        -outputresource:"${APP_TARGET_OUT_DIR}/${target}.exe"\;\#1
      COMMENT "Adding manifest...")
  endif()
endmacro()