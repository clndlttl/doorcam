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

def copyConfigFile();
    device_name = inferDiskname('/dev')
    if (device_name):
        mountDisk(device_name)
        if configIsPresent():
            readConfig()

if __name__ == '__main__':
    device_name = inferDiskname('/dev')
    if (device_name):
        mountDisk(device_name)
        if configIsPresent():
            readConfig()
