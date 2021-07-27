#
# Source files
#

# Sources shared by all executables
set(SHARED_COMMON_SRCS
  ../../include/shared/AppConfig.hpp
  ../../include/shared/AppFactory.hpp
  ../../src/shared/util/EntrypointUtil.cpp
  ../../include/shared/util/EntrypointUtil.hpp
  )

# Main executable sources
set(SHARED_SRCS
  ${SHARED_COMMON_SRCS}
  ../../src/shared/ClientManager.cpp
  ../../include/shared/ClientManager.hpp
  ../../src/shared/util/ClientUtil.cpp
  ../../include/shared/util/ClientUtil.hpp
  ../../src/shared/util/BrowserUtil.cpp
  ../../include/shared/util/BrowserUtil.hpp
  ../../src/shared/util/BrowserWindowDelegate.cpp
  ../../include/shared/util/BrowserWindowDelegate.hpp
  ../../include/shared/Entrypoint.hpp
  ../../src/shared/util/ResourceUtil.cpp
  ../../include/shared/util/ResourceUtil.hpp
  )

set(SHARED_SRCS_LINUX
  ../../src/shared/linux/ClientUtil.cpp
  ../../src/shared/linux/Entrypoint.cpp
  ../../src/shared/linux/ResourceUtil.cpp
  ../../src/shared/posix/ResourceUtil.cpp
  )

set(SHARED_SRCS_MAC
  ../../src/shared/macos/ClientUtil.mm
  ../../src/shared/macos/Entrypoint.mm
  ../../src/shared/macos/ResourceUtil.mm
  ../../src/shared/posix/ResourceUtil.cpp
  )

set(SHARED_SRCS_WINDOWS
  ../../src/shared/windows/ClientUtil.cpp
  ../../src/shared/windows/Entrypoint.cpp
  ../../src/shared/windows/ResourceUtil.cpp
  ../../src/shared/windows/BinaryResourceProvider.cpp
  ../../include/shared/util/BinaryResourceProvider.hpp
  )

append_platform_sources(SHARED_SRCS)

if(OS_MAC)
  # On macOS a separate helper executable is used for subprocesses
  set(SHARED_HELPER_SRCS
    ${SHARED_COMMON_SRCS}
    ../../src/shared/macos/ProcessEntrypoint.cpp
    ../../include/shared/macos/ProcessEntrypoint.hpp
    )
endif()

#
# Shared configuration
#

# Target static library names
set(SHARED_TARGET "shared")

if(OS_MAC)
  set(SHARED_HELPER_TARGET "shared_helper")
endif()

if(OS_LINUX OR OS_WINDOWS)
  # Logical target used to link the libcef library on Linux and Windows.
  # On macOS the CEF framework is loaded dynamically at startup.
  add_logical_target("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
endif()

include_directories(../../include)

#
# Linux configuration
#

if(OS_LINUX)
  # Static library target
  add_library(${SHARED_TARGET} STATIC ${SHARED_SRCS})
  set_app_library_target_properties(${SHARED_TARGET})
  add_dependencies(${SHARED_TARGET} libcef_dll_wrapper)
  target_link_libraries(${SHARED_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  # Copy binary and resource files to the target output directory
  copy_files("${SHARED_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${APP_TARGET_OUT_DIR}")
  copy_files("${SHARED_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${APP_TARGET_OUT_DIR}")

  # Output a message about setting SUID permissions on the chrome-sandbox target
  set_linux_suid_permissions("${SHARED_TARGET}" "${APP_TARGET_OUT_DIR}/chrome-sandbox")
endif()


#
# macOS configuration
#

if(OS_MAC)
  # Create source groups for Xcode
  set_app_source_groups("${SHARED_SRCS}")
  set_app_source_groups("${SHARED_HELPER_SRCS}")

  # Helper static library target
  add_library(${SHARED_HELPER_TARGET} STATIC ${SHARED_HELPER_SRCS})
  set_app_library_target_properties(${SHARED_HELPER_TARGET})
  add_dependencies(${SHARED_HELPER_TARGET} libcef_dll_wrapper)
  target_link_libraries(${SHARED_HELPER_TARGET} libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library
    add_logical_target("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${SHARED_HELPER_TARGET} cef_sandbox_lib)
  endif()

  # Main static library target
  add_library(${SHARED_TARGET} STATIC ${SHARED_SRCS})
  set_app_library_target_properties(${SHARED_TARGET})
  add_dependencies(${SHARED_TARGET} libcef_dll_wrapper "${SHARED_HELPER_TARGET}")
  target_link_libraries(${SHARED_TARGET} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
endif()


#
# Windows configuration
#

if(OS_WINDOWS)
  # Create source groups for Visual Studio
  set_app_source_groups("${SHARED_SRCS}")

  # Executable target
  add_library(${SHARED_TARGET} STATIC ${SHARED_SRCS})
  set_app_library_target_properties(${SHARED_TARGET})
  add_dependencies(${SHARED_TARGET} libcef_dll_wrapper)
  target_link_libraries(${SHARED_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library
    add_logical_target("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${SHARED_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
  endif()

  # Copy binary and resource files to the target output directory
  copy_files("${SHARED_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${APP_TARGET_OUT_DIR}")
  copy_files("${SHARED_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${APP_TARGET_OUT_DIR}")
endif()
