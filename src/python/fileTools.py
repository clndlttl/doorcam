# this file will be copied into /usr/lib/python3

import subprocess
import json
from time import sleep
import sys
import os

def shareVideo():
    date_list = subprocess.check_output(['date']).decode('utf-8').split(' ')
    time_list = date_list[3].split(':')
    time_str = time_list[0] + '_' + time_list[1] + '_' + time_list[2]
    date_str = date_list[1] + '_' + date_list[2] + '_' + time_str
    filename = '/share/motion_' + date_str + '.avi'

    subprocess.Popen(['mv', 'motionCapture.avi', filename])

if __name__ == '__main__':
    shareVideo()
