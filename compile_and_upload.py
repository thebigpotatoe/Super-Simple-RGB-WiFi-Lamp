#!/usr/bin/env python3
"""Small helper to update the website, compile the sketch and upload it, all in one"""

import argparse
import subprocess
from pathlib import Path
import requests

def main():
    """The magic goes here"""
    args = _get_arguments()

    git_base = Path(__file__).resolve().parent
    build_base = git_base / "build"
    firmware_file = build_base / "Super_Simple_RGB_WiFi_Lamp.ino.bin"
    sketch_file = git_base / "Super_Simple_RGB_WiFi_Lamp" / "Super_Simple_RGB_WiFi_Lamp.ino"

    print("+++ UPDATING WEBSITE ++++++++++++++++++++++++++++++")
    if subprocess.Popen([git_base/"Website"/"update_website.py"]).wait() != 0:
        raise SystemExit("Website update failed")

    print("+++ COMPILING SKETCH ++++++++++++++++++++++++++++++")
    compile_cmd = [
        "arduino", "--pref", "build.path=%s" % build_base, "--verify", sketch_file
    ]
    if subprocess.Popen(compile_cmd).wait():
        raise SystemExit("Compilation failed")

    print("+++ UPLOADING FIRMWARE (OTA) ++++++++++++++++++++++")
    with firmware_file.open("rb") as fw:
        response = requests.post("http://%s/update" % args.address, files={"update": fw})
        if "Update Success" not in response.text:
            print("Answer was: %r" % response.text)
            raise SystemExit("Upload failed")

def _get_arguments():
    parser = argparse.ArgumentParser(description="Compile and upload sketch")
    parser.add_argument("address", type=str, help="Network address of lamp")
    return parser.parse_args()

if __name__ == "__main__":
    main()
