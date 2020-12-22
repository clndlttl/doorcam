# this file will be copied into /usr/lib/python3

import subprocess
import json
from time import sleep
import sys
import os

CFG = 'doorcam_config.json'
MNT = '/media/doorcam_mount'
SAVE_CFG = '/media/doorcam_config'

def inferDiskname(path):
    disk = ''
    files = os.listdir(path)
    candidate = [f for f in files if 'sd' in f]
    candidate.sort()
    if len(candidate):
        disk = candidate[-1]
    return disk

def mountDisk(disk):
    if not os.path.exists(MNT):
        os.mkdir(MNT)
    os.system('mount /dev/' + disk + ' ' + MNT)

def configIsPresent():
    return CFG in os.listdir(MNT)

def readConfig():
    if not os.path.exists(SAVE_CFG):
        os.mkdir(SAVE_CFG)
    os.system('cp ' + MNT + '/' + CFG + ' ' + SAVE_CFG)

def copyfile():
    device_name = inferDiskname('/dev')
    if (device_name):
        mountDisk(device_name)
        if configIsPresent():
            readConfig()

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
    os.remove("iwlist.txt")

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
       
    # if the external drive is present, give it our local IP address
    if ( inferDiskname('/dev') ):
        ip = subprocess.check_output(['hostname','-I']).decode('utf-8').split()[0]
        cfg['ip'] = ip
        with open('/media/doorcam_mount/doorcam_config.json', 'w') as outfile:
            json.dump(cfg, outfile)

def configAndConnect():
    copyfile()
    connect()

if __name__ == '__main__':
    configAndConnect()
