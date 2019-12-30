class ModeColorWipe : public ModeBase
{
private:
  int colorWipePosition = -1;
  bool TurningOn = true;
  int colorWipeRed = 255;
  int colorWipeGreen = 0;
  int colorWipeBlue = 255;
  int colorWipeSpeed = 20;

public:
  ModeColorWipe() {}
  virtual void render()
  {
    EVERY_N_MILLISECONDS(colorWipeSpeed)
    {
      colorWipePosition++;
      if (TurningOn)
      {
        fill_solid(ledString, colorWipePosition, CRGB(colorWipeRed, colorWipeGreen, colorWipeBlue));
        if (colorWipePosition == NUM_LEDS)
        {
          TurningOn = false;
          colorWipePosition = -1;
        }
      }
      else
      {
        fill_solid(ledString, colorWipePosition, CRGB(0, 0, 0));
        if (colorWipePosition == NUM_LEDS)
        {
          TurningOn = true;
          colorWipePosition = -1;
        }
      }
    }
  }

  virtual void applyConfig(JsonVariant &settings)
  {
    settings["Red"] = colorWipeRed = settings["Red"] | colorWipeRed;
    settings["Green"] = colorWipeGreen = settings["Green"] | colorWipeGreen;
    settings["Blue"] = colorWipeBlue = settings["Blue"] | colorWipeBlue;
    settings["Speed"] = colorWipeSpeed = settings["Speed"] | colorWipeSpeed;
  }

  virtual const char *getName()
  {
    return "Colour Wipe";
  }

  virtual const char *getTabHtml()
  {
    return PSTR("<h2>Color Wipe Mode<\\/h2>\\r\\n"
                               "<p>Color Wipe will fill the light with a color in a wiping fashion then wipe the light away.<\\/p>\\r\\n"
                               "<div class=\\\"row my-3\\\">\\r\\n"
                               "    <input id=\\\"colorWipeSelectButton\\\" class=\\\"color col mb-2 mx-2 btn btn-lg btn-outline-light\\\" value=\\\"rgb(0,0,0)\\\"><\\/input>\\r\\n"
                               "<\\/div>\\r\\n");
  }

  virtual const char *getTabScript()
  {
    return PSTR("var colorWipeLastMessage = \\\"\\\"\\r\\n"
                                 "var colorWipeRed = 0\\r\\n"
                                 "var colorWipeGreen = 0\\r\\n"
                                 "var colorWipeBlue = 0\\r\\n"
                                 "var colorWipeDebunce = Date.now()\\r\\n"
                                 "\\r\\n"
                                 "messageEventList.push(handleColorWipeMessage)\\r\\n"
                                 "\\r\\n"
                                 "var colorWipeSelectButton = $('#colorWipeSelectButton').colorPicker({\\r\\n"
                                 "    customBG: '#222',\\r\\n"
                                 "    margin: '4px -2px 0',\\r\\n"
                                 "    doRender: 'div div',\\r\\n"
                                 "    preventFocus: true,\\r\\n"
                                 "    animationSpeed: 150,\\r\\n"
                                 "    forceAlpha: false,\\r\\n"
                                 "\\r\\n"
                                 "    // demo on how to make plugins... mobile support plugin\\r\\n"
                                 "    buildCallback: function ($elm) {\\r\\n"
                                 "        this.$colorPatch = $elm.prepend('<div class=\\\"cp-disp\\\">').find('.cp-disp');\\r\\n"
                                 "    },\\r\\n"
                                 "    cssAddon: '.cp-disp {padding:10px; margin-bottom:6px; font-size:19px; height:20px; line-height:20px}' +\\r\\n"
                                 "        '.cp-xy-slider {width:200px; height:200px;}' +\\r\\n"
                                 "        '.cp-xy-cursor {width:16px; height:16px; border-width:2px; margin:-8px}' +\\r\\n"
                                 "        '.cp-z-slider {height:200px; width:40px;}' +\\r\\n"
                                 "        '.cp-z-cursor {border-width:8px; margin-top:-8px;}' +\\r\\n"
                                 "        '.cp-alpha {height:40px;}' +\\r\\n"
                                 "        '.cp-alpha-cursor {border-width:8px; margin-left:-8px;}',\\r\\n"
                                 "\\r\\n"
                                 "    renderCallback: function ($elm, toggled) {\\r\\n"
                                 "        var colors = this.color.colors;\\r\\n"
                                 "        onColorWipeButtonEvent(Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                 "\\r\\n"
                                 "        setPickerColor(colorWipeSelectButton, Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                 "    }\\r\\n"
                                 "})\\r\\n"
                                 "\\r\\n"
                                 "function handleColorWipeMessage(jsonMessage) {\\r\\n"
                                 "    if (\\\"Color Wipe\\\" in jsonMessage) {\\r\\n"
                                 "        jsonMessage = jsonMessage[\\\"Color Wipe\\\"]\\r\\n"
                                 "        if (typeof jsonMessage === \\\"object\\\") {\\r\\n"
                                 "            var newRed = currentRed\\r\\n"
                                 "            var newGreen = currentGreen\\r\\n"
                                 "            var newBlue = currentBlue\\r\\n"
                                 "\\r\\n"
                                 "            if ((\\\"Red\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.Red === \\\"number\\\") {\\r\\n"
                                 "                    newRed = jsonMessage.Red\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"Green\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.Green === \\\"number\\\") {\\r\\n"
                                 "                    newGreen = jsonMessage.Green\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"Blue\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.Blue === \\\"number\\\") {\\r\\n"
                                 "                    newBlue = jsonMessage.Blue\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            setPickerColor(colorWipeSelectButton, newRed, newGreen, newBlue);\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n"
                                 "\\r\\n"
                                 "function onColorWipeButtonEvent(red, green, blue) {\\r\\n"
                                 "    if (currentRed != red || currentGreen != green || currentBlue != blue) {\\r\\n"
                                 "        currentRed = red\\r\\n"
                                 "        currentGreen = green\\r\\n"
                                 "        currentBlue = blue\\r\\n"
                                 "\\r\\n"
                                 "        msg = {\\r\\n"
                                 "            \\\"State\\\": true,\\r\\n"
                                 "            \\\"Mode\\\": \\\"Color Wipe\\\",\\r\\n"
                                 "            \\\"Color Wipe\\\": {\\r\\n"
                                 "                \\\"Red\\\": red,\\r\\n"
                                 "                \\\"Green\\\": green,\\r\\n"
                                 "                \\\"Blue\\\": blue\\r\\n"
                                 "            }\\r\\n"
                                 "        }\\r\\n"
                                 "\\r\\n"
                                 "        if (Date.now() - colorWipeDebunce > 50) {\\r\\n"
                                 "            colorWipeDebunce = Date.now()\\r\\n"
                                 "            sendMessage(msg)\\r\\n"
                                 "        }\\r\\n"
                                 "        if (msg != colorWipeLastMessage && Date.now() - colorWipeDebunce > 50) {\\r\\n"
                                 "            colorWipeDebunce = Date.now()\\r\\n"
                                 "            colorWipeLastMessage = msg\\r\\n"
                                 "            sendMessage(msg)\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n");
  }
};
