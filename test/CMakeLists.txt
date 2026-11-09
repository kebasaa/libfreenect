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

# freenect-v4l2's pixel conversions (examples/v4l2_convert.h)
add_executable(test_v4l2_convert test_v4l2_convert.c)
target_include_directories(test_v4l2_convert PRIVATE ${CMAKE_SOURCE_DIR}/examples)
add_test(NAME v4l2_convert COMMAND test_v4l2_convert)

# Isochronous stream health check and VM detection (src/iso_diag.c)
add_executable(test_iso_diag test_iso_diag.c ${CMAKE_SOURCE_DIR}/src/iso_diag.c)
add_test(NAME iso_diag COMMAND test_iso_diag)

# The viewers' GLFW window layer (examples/fn_glut.c): a close request reaches
# the viewer as Esc, and a viewer that is stuck is ended by the watchdog.
# Needs a display (CI uses xvfb-run); skipped without one.
if (BUILD_EXAMPLES AND FREENECT_GL_BACKEND STREQUAL "GLFW" AND UNIX AND NOT APPLE)
  add_executable(test_fn_glut test_fn_glut.c ${CMAKE_SOURCE_DIR}/examples/fn_glut.c)
  target_compile_definitions(test_fn_glut PRIVATE FN_USE_GLFW)
  target_include_directories(test_fn_glut PRIVATE ${CMAKE_SOURCE_DIR}/examples)
  target_link_libraries(test_fn_glut glfw "${OPENGL_TARGET}")
  add_test(NAME fn_glut_close COMMAND test_fn_glut normal)
  # killed by SIGALRM: the shell reports 128 + 14
  add_test(NAME fn_glut_watchdog
           COMMAND sh -c "\"$<TARGET_FILE:test_fn_glut>\" blocked; c=$?; [ $c -eq 77 ] && exit 77; [ $c -eq 142 ]")
  set_tests_properties(fn_glut_close fn_glut_watchdog PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 20)
endif ()

# Frame reassembly from the camera's packets (src/stream.c): only complete
# frames are delivered, and the first video frames after a start are skipped.
add_executable(test_stream test_stream.c ${CMAKE_SOURCE_DIR}/src/stream.c)
add_test(NAME stream COMMAND test_stream)

# 320x240 and 80x60 depth (src/depth_decimate.h): the same pixels as
# Microsoft's runtime, mirrored and unmirrored.
add_executable(test_decimate test_decimate.c)
add_test(NAME decimate COMMAND test_decimate)
