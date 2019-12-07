# Modifying the website

The leading file for the website is `Website.html`. You can change this file with the
editor of your choice and "dry test" it in your browser locally.

To make the website work with your lamp, you will have to compile it into the firmware
of your lamp. This is done in two steps:

- Convert the `Website.html` to `Super_Simple_RGB_WiFi_Lamp/Web_Page.ino`
- Compile and upload the application

The first step can be made using the script `update_website.py` in this directory.
You need to have Python 3 installed to be able to use it.

## Manual copying

If you don't have Python 3 installed, you can convert the contents of Website.html on
your own using your editor by doing the following steps:

- find `"` and replace with `\"`
- find the line beginnging `^` (regex) and replace with `  "`
- find the line ending `$` (regex) and replace with `\\n"`
- then copy the text over to `Super_Simple_RGB_WiFi_Lamp/Web_Page.ino`
