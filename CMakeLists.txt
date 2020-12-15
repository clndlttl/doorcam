cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.9")

project(DOORCAM)

add_subdirectory(server)
add_subdirectory(client)

# for launching on boot
install(FILES doorcam.service
        DESTINATION /etc/systemd/system)

# move python files
install(FILES src/python/doorcam_init.py src/python/fileTools.py
        DESTINATION /usr/lib/python3)
