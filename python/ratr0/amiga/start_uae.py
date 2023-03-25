#!/usr/bin/env python3

"""
Start script to create an ADF image and execute FS-UAE from the engine
with that image
"""
import subprocess
import os
import jinja2


FSUAE_TEMPLATE = """
[fs-uae]
amiga_model = A500/512K
floppy_drive_0 = /Users/weiju/Amiga/adfs/amos_cover.adf
joystick_port_0 = Mouse
joystick_port_1 = none
kickstart_file = amiga-os-130.rom
floppy_drive_volume = 0
ntsc_mode = 1
save_states = 0
"""

if __name__ == '__main__':
    config_path = "/tmp/engine_start.fs-uae"
    with open(config_path, "w") as outfile:
        outfile.write(FSUAE_TEMPLATE)

    run_state = subprocess.run(["fs-uae", config_path])
