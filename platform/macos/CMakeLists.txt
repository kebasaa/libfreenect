######################################################################################
# macOS integration: launchd agent that loads the audio firmware at plug-in
######################################################################################
#
# Installed to share/libfreenect/macos; the user (or a package) copies it to
# ~/Library/LaunchAgents or /Library/LaunchAgents, see README.md.

set (FWLOADER_PATH "${CMAKE_INSTALL_PREFIX}/bin/freenect-fwloader")
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/launchd/org.openkinect.fwloader.plist.in"
                "${CMAKE_CURRENT_BINARY_DIR}/org.openkinect.fwloader.plist" @ONLY)

if (BUILD_EXAMPLES)
  # The agent runs freenect-fwloader, which is built with the examples.
  install (FILES "${CMAKE_CURRENT_BINARY_DIR}/org.openkinect.fwloader.plist"
           DESTINATION "share/libfreenect/macos")
endif ()
install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
         DESTINATION "share/doc/libfreenect/macos")
