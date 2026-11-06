######################################################################################
# Unit tests that need no Kinect. Run them with: ctest --test-dir build -C Release
######################################################################################

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

# Hotplug bookkeeping (src/hotplug.c)
add_executable(test_hotplug test_hotplug.c ${CMAKE_SOURCE_DIR}/src/hotplug.c)
add_test(NAME hotplug COMMAND test_hotplug)

# The hotplug path of src/core.c against a simulated USB layer (the test
# provides the fnusb_* and camera functions that core.c calls)
add_executable(test_core_hotplug test_core_hotplug.c ${CMAKE_SOURCE_DIR}/src/core.c ${CMAKE_SOURCE_DIR}/src/hotplug.c)
add_test(NAME core_hotplug COMMAND test_core_hotplug)
