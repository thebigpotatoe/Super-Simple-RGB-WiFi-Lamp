# Mode Registry

In this folder are all the modes created by contributors which work out of the box with the sketch.
If you would like to add these modes simply follow the directions below.

## Installing Modes

#### Copy Mode .ino file to the main sketch folder

Simply move the .ino file of your choice from the __External Modes__ folder into the sketch main
folder. This will be the "__Super_Simple_RGB_WiFi_Lamp__" folder with the main .ino file.

![MoveFiles](../Pictures/MoveFiles.png)

#### Install any new libraries mentioned

Some modes may require a new library to be installed. This should be mentioned at the top of any new
mode file. You will need to install these before the mode will work. For example; the visualiser
mode requires the FFT library to be installed before it will work.

![MoveFiles](../Pictures/InstallLibraries.png)

#### Add mode to the mode registry in the sketch

In the `mode_registry.ino` file you must add your mode so that the sketch knows it should use it.
Failure to do so will result in your mode not being rendered. To add the mode in the registry simply
copy and paste the following replacing `NEW_MODE_NAME` with the name of the mode, and
`NEW_MODE_CONSTRUCTOR` with the name of the mode as well (this must match details in the mode's .ino
file).

```
modes["NEW_MODE_NAME"] = new NEW_MODE_CONSTRUCTOR();
```

#### Compile and upload

After adding the file to the main sketch folder, installing any new libraries that are required, and
adding the mode to the registry, simply compile and upload the sketch to your ESP8266.

#### Issues with new modes

If you run into any issues with new modes, please create an issue and provide as much information as
possible so we can help identify the issue.

## Creating New Modes

While there is no guide currently on how to add a mode yourself, you may be able to create one by
reading how one of the other modes is implemented. I suggest having a read of how the colour mode is
implemented first.

## TODO

- Show how to add the mode in the arduino environment
- Show how to add the mode to the webpage
- Make a new mode pull request template
