project(doorcam_server)

add_executable(
  doorcam_server
  ${CMAKE_SOURCE_DIR}/src/main_server.cpp
  ${CMAKE_SOURCE_DIR}/src/server.cpp
  ${CMAKE_SOURCE_DIR}/src/socket.cpp
  ${CMAKE_SOURCE_DIR}/src/pyfunc.cpp
  ${CMAKE_SOURCE_DIR}/src/Camera.cpp
  ${CMAKE_SOURCE_DIR}/src/Config.cpp
  ${CMAKE_SOURCE_DIR}/src/Console.cpp
)

find_package(OpenCV REQUIRED COMPONENTS
        core imgproc highgui videoio)

# ATTN: /usr/include/python3 is a symbolic link that you
# must create to point at /usr/include/python3.X
target_include_directories(
  doorcam_server PUBLIC
  ${CMAKE_SOURCE_DIR}/src/inc
  ${OpenCV_INCLUDE_DIRS}
  /usr/include/python3
)

# ATTN: /usr/lib/libpython3 is a symbolic link that you
# must create to point at libpython3.Xm.so, wherever
# it may be
target_link_libraries(
  doorcam_server
  ${OpenCV_LIBS}
  "/usr/lib/libpython3"
  pthread
)

install(TARGETS doorcam_server
        DESTINATION ../sbin)

