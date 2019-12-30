#!/usr/bin/env python3
"""Small helper to create the Website.ino from Website/Website.html"""

from pathlib import Path

def main():
    """Main entry point to do the conversion"""

    git_base = Path(__file__).resolve().parent.parent
    source_file = git_base / "Website" / "Class_HTML_Input.html"
    target_file = git_base / "Website" / "Exported_Class_HTML.ino"

    with source_file.open(encoding="utf-8") as source:
        with target_file.open("w", encoding="utf-8") as target:
            target.write("const char *tabHtml = PSTR(")

            for line in source:
                target.write("\"%s\\\\r\\\\n\"\n" % line.replace('\\', '\\\\').replace('"', '\\\\\\"').replace('/', '\\\\/').rstrip())

            target.write(");")

    print("Exported_Class_HTML.ino updated:", target_file)


if __name__ == "__main__":
    main()
