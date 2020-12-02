project(NetworkBringup)

add_executable(
  networkbringup
  ${CMAKE_SOURCE_DIR}/src/main_network.cpp
  ${CMAKE_SOURCE_DIR}/src/pyfunc.cpp
)

target_include_directories(
  networkbringup PUBLIC
  ${CMAKE_SOURCE_DIR}/src/inc
  /usr/include/python3.4
)

target_link_libraries(
  networkbringup
  "/usr/lib/arm-linux-gnueabihf/libpython3.4m.so"
)
