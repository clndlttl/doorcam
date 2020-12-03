project(doorcam_server)

add_executable(
  doorcam_server
  ${CMAKE_SOURCE_DIR}/src/main_server.cpp
  ${CMAKE_SOURCE_DIR}/src/server.cpp
  ${CMAKE_SOURCE_DIR}/src/socket.cpp
)

find_package(OpenCV REQUIRED COMPONENTS
        core imgproc highgui videoio)

target_include_directories(
  doorcam_server PUBLIC
  ${CMAKE_SOURCE_DIR}/src/inc
  ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(
  doorcam_server
  ${OpenCV_LIBS}
)
