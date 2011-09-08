CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(digimesh)

SET(CMAKE_BUILD_TYPE Debug)

SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

INCLUDE_DIRECTORIES(include)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/git/asio-serial-device/cmake")
FIND_PACKAGE(ASIO_SERIAL_DEVICE REQUIRED)
INCLUDE_DIRECTORIES(${ASIO_SERIAL_DEVICE_INCLUDE_DIRS})

FIND_PACKAGE(Boost COMPONENTS system program_options thread REQUIRED)

ADD_LIBRARY(digimesh SHARED
  src/DigimeshAPIFrame.cc
  src/DigimeshATCommand.cc
  src/DigimeshBase.cc)
TARGET_LINK_LIBRARIES(digimesh
  ${ASIO_SERIAL_DEVICE_LIBRARIES}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY})

ADD_EXECUTABLE(test_digimesh_api_frame src/test_digimesh_api_frame.cc)
TARGET_LINK_LIBRARIES(test_digimesh_api_frame
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  digimesh)

ADD_EXECUTABLE(set_digimesh_parameters src/set_digimesh_parameters.cc)
TARGET_LINK_LIBRARIES(set_digimesh_parameters
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  digimesh)

INSTALL(TARGETS digimesh DESTINATION lib)
INSTALL(TARGETS test_digimesh_api_frame DESTINATION bin)
INSTALL(TARGETS set_digimesh_parameters DESTINATION bin)
