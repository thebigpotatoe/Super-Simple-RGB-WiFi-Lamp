#!/usr/bin/env python3
"""Small helper to create the Website.ino from Website/Website.html"""

from pathlib import Path

def main():
    """Main entry point to do the conversion"""

    git_base = Path(__file__).resolve().parent.parent
    source_file = git_base / "Website" / "Website.html"
    target_file = git_base / "Super_Simple_RGB_WiFi_Lamp" / "Web_Page.ino"

    with source_file.open(encoding="utf-8") as source:
        with target_file.open("w", encoding="utf-8") as target:
            target.write("// Created by Website/update_website.py\n"
                         "const char websiteSource[] PROGMEM =\n")

            for line in source:
                target.write("  \"%s\\n\"\n" % line.replace('"', '\\"').rstrip())

            target.write(";\n")

    print("File updated:", target_file)
    print("You may now recompile your application.")


if __name__ == "__main__":
    main()
