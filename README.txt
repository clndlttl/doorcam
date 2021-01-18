$$$$$$$\                                 $$$$$$\                          
$$  __$$\                               $$  __$$\                         
$$ |  $$ | $$$$$$\   $$$$$$\   $$$$$$\  $$ /  \__| $$$$$$\  $$$$$$\$$$$\  
$$ |  $$ |$$  __$$\ $$  __$$\ $$  __$$\ $$ |       \____$$\ $$  _$$  _$$\ 
$$ |  $$ |$$ /  $$ |$$ /  $$ |$$ |  \__|$$ |       $$$$$$$ |$$ / $$ / $$ |
$$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |      $$ |  $$\ $$  __$$ |$$ | $$ | $$ |
$$$$$$$  |\$$$$$$  |\$$$$$$  |$$ |      \$$$$$$  |\$$$$$$$ |$$ | $$ | $$ |
\_______/  \______/  \______/ \__|       \______/  \_______|\__| \__| \__|


Welcome! DoorCam is an open-source repository that lets you turn a USB
webcam and Raspberry Pi with WiFi dongle into a wireless camera accessbile
on a local network.

Currently, you'll need a linux computer with which to connect to the RPi
and view its image stream. Looking for open-source collaborators who'd
like to add client apps for MacOS, Windows, or mobile device. If
interested, let me know at https://www.colinfoxdsp.com/contact


####### DEPENDENCIES #######

 1. g++ supporting C++14
 2. cmake, version 3.6 or newer
 3. OpenCV4
 4. boost
 5. python3
    a. create a sym link /usr/include/python3 -> /usr/include/python3.Xm
    b. create a sym link /usr/lib/libpython3  -> path-to-libpython3.Xm.so


####### INSTALL GUIDE #######

Clone this repo to your RPi and linux client machine.

 1. cd to root of this repo
    a. mkdir build && cd build && cmake ..
    b. make
    c. sudo make install

 2. Enable boot service:
    a. sudo systemctl daemon-reload
    b. sudo systemctl enable doorcam.service
    c. (optional, if deploying camera on a different WiFi network than what
       is used for this install procedure) sudo systemctl enable findUSB.service

 3. Take note of the ip address of your RPi. With only the USB webcam and WiFi
    dongle plugged in, plug the RPi into the wall and position camera wherever you
    like in your home network.

 4. On your client computer, run /build/client/doorcam_client <ip address of RPi>
    - The video stream should appear in an OpenCV window.
    - You may send commands to the camera on the command line:
      * quit                 stops doorcam service on the RPi.
      * size(width,height)   changes image resolution to width x height
                             note: resolution depends on camera!
      * close                closes the client app. RPi camera server is still
                             running.

####### SWITCHING WIFI NETWORKS #######

If the camera is moved to a WiFi network other than the one used for this
configuration, there is no need to attach a monitor and keyboard to give
it the ssid and passkey of the new network.

Steps:
 - make sure you enabled findUSB.service (step 2.c above)

 - edit file doorcam/server/doorcam_config.json to contain the ssid and
   passkey of the new network, save it to a thumbdrive and plug that into the
   Pi before plugging it into the wall

 - a minute after the RPi boots, remove the thumbdrive; the .json file will
   have a new entry containing the local ip address given to the Pi. If this
   ip address entry isn't there, something went wrong -- a monitor and
   keyboard will be needed

 - use that ip address when running the client app


*******************
KNOWN ISSUES/BUGS:

*******************
