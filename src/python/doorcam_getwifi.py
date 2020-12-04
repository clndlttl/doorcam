# this file must be copied into /usr/lib/python3

import subprocess
import json
from time import sleep
import sys

def connect():
    # read in the creds from /media/doorcam_config/doorcam_config.json
    file = open("/media/doorcam_config/doorcam_config.json", "r")
    text = file.read()
    cfg = json.loads(text)

    # make sure that network is visible to wlan0
    iwlist = open("iwlist.txt", "w+")
    subprocess.Popen(["iwlist", "wlan0", "scan"], stdout=iwlist)
    iwlist.close()
    sleep(1)

    iwlist = open("iwlist.txt", "r")
    output = iwlist.read()
    iwlist.close()

    if cfg['ssid'] not in output:
        print('doorcam: ssid from .json is not visible')
        sys.exit(0)

    # check to see if creds are already added to wpa_supplicant.conf
    wpa = open("/etc/wpa_supplicant/wpa_supplicant.conf", "r")
    creds = wpa.read()
    wpa.close()

    if cfg['ssid'] not in creds:
        print('doorcam: adding network...')
        sleep(1)
        wpa = open("/etc/wpa_supplicant/wpa_supplicant.conf", "a+")
        wpa.write("\nnetwork={{\n\tssid=\"{}\"\n\tpsk=\"{}\"\n}}\n".format(cfg['ssid'], cfg['psk']))
        wpa.close()
        sleep(1)
        subprocess.Popen(["wpa_cli", "-i", "wlan0", "reconfigure"])

if __name__ == '__main__':
    connect()
