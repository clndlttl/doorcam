Welcome to DOORCAM!

The idea:
- plug a USB webcam and WiFi dongle into a Raspberry Pi.
- edit a .json file to select your configuration, save it to
  a thumbdrive and plug that into the Pi too.
- plug the Pi into the wall, mount the camera where you like.
- remove the thumbdrive; the .json file will have a new entry
  containing the local ip address of the Pi.
- on another machine, run a client app that either streams
  live video to you from your wireless camera, or, lets you
  view saved videos captured by the motion detector

Example doorcam_config.json (see ./server/doorcam_config.json):
{
	"ssid" : "YourNetworkNameHere",
    "psk"  : "YourPasswordHere"
}

The bare-bones .json file above will put the camera in server
mode. To enable motion detection mode instead, add this line:
    "mode" : "motion"

In motion mode, time-stamped video files will be saved to the
thumbdrive, for later retrieval.

Other optional lines that can be added to the .json file:
    "fps" : "10.0",
    "imgWidth"  : "320",
    "imgHeight" : "240"

fps stands for frames per second, floating-point.
imgWidth and imgHeight set the resolution of the images (depends
on your camera).

This repo really isn't quite ready for deployment, but it's close.
If you are an employer, please have a look around, but know that
this is a work in progress.

Thanks,
Colin Fox

*******************

BUGS:
- .avi video file isn't getting saved in boot service

*******************

Install procedure:

1. clone this repo
2. install OpenCV4
3. install boost
4. install python3.X (X is some version number)
   a. create a symbolic link /usr/include/python3 -> /usr/include/python3.Xm
   b. create a symbolic link /usr/lib/libpython3  -> path-to-libpython3.Xm.so
5. make sure sudo can run python files
   a. my require adding the following to /etc/sudoers:
      Default env-keep += "PYTHONPATH"
      with PYTHONPATH=/usr/lib/python3
6. cd to root of this repo
   a. cmake .
   b. make
   c. sudo make install
7. Enable the doorcam service:
   a. sudo systemctl daemon-reload
   b. sudo systemctl enable doorcam.service
8. Edit doorcam_config.json, put it on a USB drive, insert into PI with
   webcam and wifi dongle, plug it in and enjoy

