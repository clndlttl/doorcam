Welcome to DOORCAM!

This repo really isn't ready for deployment, but it's close.
If you are an employer, please have a look around, but know
that this is a work in progress.

Thanks,
Colin Fox

TODO:

- CMake install
- console port
- write video

Requirements (for install process):

- copy src/python/doorcam_init.py to /usr/lib/python3
- for raspi, copy server/server executable to /usr/sbin
- move startup.service to /etc/systemd/system

- make sure sudo can run doorcam_init
  - my require adding the following to /etc/sudoers:
    Default env-keep += "PYTHONPATH"
    - my need to have this is .bashrc:
      export PYTHONPATH=/usr/lib/python3

