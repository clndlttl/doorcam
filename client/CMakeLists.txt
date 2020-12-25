project(client)

add_executable(
  doorcam_client
  ${CMAKE_SOURCE_DIR}/src/main_client.cpp
  ${CMAKE_SOURCE_DIR}/src/client.cpp
  ${CMAKE_SOURCE_DIR}/src/socket.cpp
)

find_package(OpenCV REQUIRED COMPONENTS
        core imgproc highgui videoio)

target_include_directories(
  doorcam_client PUBLIC
  ${CMAKE_SOURCE_DIR}/src/inc
  ${OpenCV_INCLUDE_DIRS}
  /usr/include/boost/property_tree
)

target_link_libraries(
  doorcam_client
  ${OpenCV_LIBS}
  pthread
)
