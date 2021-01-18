cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(DOORCAM)

add_subdirectory(server)
add_subdirectory(client)

# look for usb drive with .json config at boot
install(FILES findUSB.service
        DESTINATION /etc/systemd/system)

# for launching on boot
install(FILES doorcam.service
        DESTINATION /etc/systemd/system)

# move python files
install(FILES src/python/doorcam_init.py
        DESTINATION /usr/lib/python3)
