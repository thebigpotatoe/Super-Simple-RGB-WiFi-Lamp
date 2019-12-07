class ModeBellCurve : public ModeBase
{
private:
  int bellCurveRed = 128;
  int bellCurveGreen = 128;
  int bellCurveBlue = 128;

public:
  ModeBellCurve() {}
  virtual void render()
  {
    // Set the top brightness
    for (int i = 0; i < topNumLeds; i++)
    {
      int ledNrightness = cubicwave8((255 / (float)topNumLeds) * i);
      ledString[topLeds[i]] = CRGB(bellCurveRed, bellCurveGreen, bellCurveBlue);
      ledString[topLeds[i]] %= ledNrightness;
    }

    // Set the Bottom brightness
    for (int i = 0; i < bottomNumLeds; i++)
    {
      int ledNrightness = cubicwave8((255 / (float)bottomNumLeds) * i);
      ledString[bottomLeds[i]] = CRGB(bellCurveRed, bellCurveGreen, bellCurveBlue);
      ledString[bottomLeds[i]] %= ledNrightness;
    }
  }

  virtual void applyConfig(JsonVariant &settings)
  {
    settings["Red"] = bellCurveRed = settings["Red"] | bellCurveRed;
    settings["Green"] = bellCurveGreen = settings["Green"] | bellCurveGreen;
    settings["Blue"] = bellCurveBlue = settings["Blue"] | bellCurveBlue;
  }

  virtual void sendWebsiteData(WebSocketsServer &_webSocketServer)
  {
    const char *modeName = "Bell Curve";
    const char *tabHtml = PSTR("<h2>Bell Curve Mode<\\/h2>\\r\\n"
                               "<p>In this mode the lamp will shape the light into a bell curve. This is meant to be more asthetically\\r\\n"
                               "    pleasing than the regular colour mode.<\\/p>\\r\\n"
                               "<div class=\\\"row my-3\\\">\\r\\n"
                               "    <input id=\\\"bellCurveSelectButton\\\" class=\\\"color col mb-2 mx-2 btn btn-lg btn-outline-light\\\" value=\\\"rgb(0,0,0)\\\">\\r\\n"
                               "<\\/div>\\r\\n");
    const char *tabScript = PSTR("bellCurveDebunce = Date.now()\\r\\n"
                                 "var bellRed = 0\\r\\n"
                                 "var bellGreen = 0\\r\\n"
                                 "var bellBlue = 0\\r\\n"
                                 "\\r\\n"
                                 "messageEventList.push(handleBellCurveMessage)\\r\\n"
                                 "\\r\\n"
                                 "var bellCurveSelectButton = $('#bellCurveSelectButton').colorPicker({\\r\\n"
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
                                 "        onOnBellCurvePickerEvent(Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                 "        setPickerColor(bellCurveSelectButton, Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                 "    }\\r\\n"
                                 "})\\r\\n"
                                 "\\r\\n"
                                 "function handleBellCurveMessage(jsonMessage) {\\r\\n"
                                 "    if (\\\"Bell Curve\\\" in jsonMessage) {\\r\\n"
                                 "        jsonMessage = jsonMessage[\\\"Bell Curve\\\"]\\r\\n"
                                 "        if (typeof jsonMessage === \\\"object\\\") {\\r\\n"
                                 "            var newRed = bellRed\\r\\n"
                                 "            var newGreen = bellGreen\\r\\n"
                                 "            var newBlue = bellBlue\\r\\n"
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
                                 "\\r\\n"
                                 "            setPickerColor(bellCurveSelectButton, newRed, newGreen, newBlue);\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n"
                                 "\\r\\n"
                                 "function onOnBellCurvePickerEvent(red, green, blue) {\\r\\n"
                                 "    if (bellRed != red || bellGreen != green || bellBlue != blue) {\\r\\n"
                                 "        msg = {\\r\\n"
                                 "            \\\"State\\\": true,\\r\\n"
                                 "            \\\"Mode\\\": \\\"Bell Curve\\\",\\r\\n"
                                 "            \\\"Bell Curve\\\": {\\r\\n"
                                 "                \\\"Red\\\": red,\\r\\n"
                                 "                \\\"Green\\\": green,\\r\\n"
                                 "                \\\"Blue\\\": blue\\r\\n"
                                 "            }\\r\\n"
                                 "        }\\r\\n"
                                 "\\r\\n"
                                 "        if (Date.now() - bellCurveDebunce > 50) {\\r\\n"
                                 "            bellCurveDebunce = Date.now()\\r\\n"
                                 "            sendMessage(msg)\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n");

    String htmlJSON = String("{\"Tab\" : {") + "\"Name\": \"" + modeName + "\", \"tabHtml\" : \"" + tabHtml + "\", \"tabScript\" : \"" + tabScript + "\"}}";

    _webSocketServer.broadcastTXT(htmlJSON.c_str());
  }
};
