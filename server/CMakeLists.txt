project(doorcam_server)

add_executable(
  server
  ${CMAKE_SOURCE_DIR}/src/main_server.cpp
  ${CMAKE_SOURCE_DIR}/src/server.cpp
  ${CMAKE_SOURCE_DIR}/src/socket.cpp
)

find_package(OpenCV REQUIRED COMPONENTS
        core imgproc highgui videoio)

target_include_directories(
  server PUBLIC
  ${CMAKE_SOURCE_DIR}/src/inc
  ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(
  server
  ${OpenCV_LIBS}
)
