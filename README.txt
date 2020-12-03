Requirements (try to automate):

- copy src/python/doorcam_init.py to /usr/lib/python3
- for raspi, copy server/server executable to /usr/sbin
- move startup.service to /etc/systemd/system

- make sure sudo can run doorcam_init
  - my require adding the following to /etc/sudoers:
    Default env-keep += "PYTHONPATH"
    - my need to have this is .bashrc:
      export PYTHONPATH=/usr/lib/python3
