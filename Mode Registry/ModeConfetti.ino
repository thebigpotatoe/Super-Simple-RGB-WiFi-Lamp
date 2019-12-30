class ModeConfetti : public ModeBase
{
private:
  bool confettiActive = true;
  int confettiSpeed = 100;
  int confettiPixel = random(NUM_LEDS);

public:
  ModeConfetti() {}
  virtual void render()
  {
    EVERY_N_MILLISECONDS(confettiSpeed)
    {
      if (confettiActive)
      {
        confettiPixel = random(NUM_LEDS);
        fadeToBlackBy(ledString, NUM_LEDS, 10);
        uint8_t pos = random8(NUM_LEDS);
        ledString[pos] += CHSV(random8(), random8(), random8());
      }
    }
  }

  virtual void applyConfig(JsonVariant &settings)
  {
    // Are not used
    //settings["Red"] = confettiRed = settings["Red"] | confettiRed;
    //settings["Green"]= confettiGreen = settings["Green"] | confettiGreen;
    //settings["Blue"] = confettiBlue = settings["Blue"] | confettiBlue;
    settings["Speed"] = confettiSpeed = settings["Speed"] | confettiSpeed;
  }

  virtual const char *getName()
  {
    return "Confetti";
  }

  virtual const char *getTabHtml()
  {
    return PSTR("<h2>Confetti Mode<\\/h2>\\r\\n"
                "<p>Confetti will flash random colors to emulate confetti.<\\/p>\\r\\n");
  }

  virtual const char *getTabScript()
  {
    return PSTR("var confettiLastMessage = \\\"\\\"\\r\\n"
                                 "                var confettiRed = 0;\\r\\n"
                                 "                var confettiGreen = 0;\\r\\n"
                                 "                var confettiBlue = 0;\\r\\n"
                                 "                var confettiDebunce = Date.now()\\r\\n"
                                 "\\r\\n"
                                 "                messageEventList.push(handleConfettiMessage)\\r\\n"
                                 "\\r\\n"
                                 "                var confettiSelectButton = $('#confettiSelectButton').colorPicker({\\r\\n"
                                 "                    customBG: '#222',\\r\\n"
                                 "                    margin: '4px -2px 0',\\r\\n"
                                 "                    doRender: 'div div',\\r\\n"
                                 "                    preventFocus: true,\\r\\n"
                                 "                    animationSpeed: 150,\\r\\n"
                                 "                    forceAlpha: false,\\r\\n"
                                 "\\r\\n"
                                 "                    // demo on how to make plugins... mobile support plugin\\r\\n"
                                 "                    buildCallback: function ($elm) {\\r\\n"
                                 "                        this.$colorPatch = $elm.prepend('<div class=\\\"cp-disp\\\">').find('.cp-disp');\\r\\n"
                                 "                    },\\r\\n"
                                 "                    cssAddon:\\r\\n"
                                 "                        '.cp-xy-slider {width:200px; height:200px;}' +\\r\\n"
                                 "                        '.cp-xy-cursor {width:16px; height:16px; border-width:2px; margin:-8px}' +\\r\\n"
                                 "                        '.cp-z-slider {height:200px; width:40px;}' +\\r\\n"
                                 "                        '.cp-z-cursor {border-width:8px; margin-top:-8px;}' +\\r\\n"
                                 "                        '.cp-alpha {height:40px;}' +\\r\\n"
                                 "                        '.cp-alpha-cursor {border-width:8px; margin-left:-8px;}',\\r\\n"
                                 "\\r\\n"
                                 "                    renderCallback: function ($elm, toggled) {\\r\\n"
                                 "                        var colors = this.color.colors;\\r\\n"
                                 "                        onConfettiButtonEvent(Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                 "                        setPickerColor(confettiSelectButton, Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                 "                    }\\r\\n"
                                 "                })\\r\\n"
                                 "\\r\\n"
                                 "                function onConfettiEvent() {\\r\\n"
                                 "                    let currentSpeedValue = parseFloat($(\\\"#confettiSpeed\\\").val())\\r\\n"
                                 "\\r\\n"
                                 "                    $(\\\"#confettiSpeedLabel\\\").html(currentSpeedValue)\\r\\n"
                                 "\\r\\n"
                                 "                    msg = {\\r\\n"
                                 "                        \\\"State\\\": true,\\r\\n"
                                 "                        \\\"Mode\\\": \\\"Confetti\\\",\\r\\n"
                                 "                        \\\"Confetti\\\": {\\r\\n"
                                 "                            \\\"Speed\\\": currentSpeedValue,\\r\\n"
                                 "                        }\\r\\n"
                                 "                    }\\r\\n"
                                 "\\r\\n"
                                 "                    if (msg != confettiLastMessage && Date.now() - confettiDebunce > 50) {\\r\\n"
                                 "                        confettiDebunce = Date.now()\\r\\n"
                                 "                        confettiLastMessage = msg\\r\\n"
                                 "                        sendMessage(msg)\\r\\n"
                                 "                    }\\r\\n"
                                 "                }\\r\\n"
                                 "\\r\\n"
                                 "                function handleConfettiMessage(jsonMessage) {\\r\\n"
                                 "                    if (\\\"Confetti\\\" in jsonMessage) {\\r\\n"
                                 "                        jsonMessage = jsonMessage.Confetti\\r\\n"
                                 "                        if (typeof jsonMessage === \\\"object\\\") {\\r\\n"
                                 "                            var newRed = currentRed\\r\\n"
                                 "                            var newGreen = currentGreen\\r\\n"
                                 "                            var newBlue = currentBlue\\r\\n"
                                 "\\r\\n"
                                 "                            if ((\\\"Red\\\" in jsonMessage)) {\\r\\n"
                                 "                                if (typeof jsonMessage.Red === \\\"number\\\") {\\r\\n"
                                 "                                    newRed = jsonMessage.Red\\r\\n"
                                 "                                }\\r\\n"
                                 "                            }\\r\\n"
                                 "                            if ((\\\"Green\\\" in jsonMessage)) {\\r\\n"
                                 "                                if (typeof jsonMessage.Green === \\\"number\\\") {\\r\\n"
                                 "                                    newGreen = jsonMessage.Green\\r\\n"
                                 "                                }\\r\\n"
                                 "                            }\\r\\n"
                                 "                            if ((\\\"Blue\\\" in jsonMessage)) {\\r\\n"
                                 "                                if (typeof jsonMessage.Blue === \\\"number\\\") {\\r\\n"
                                 "                                    newBlue = jsonMessage.Blue\\r\\n"
                                 "                                }\\r\\n"
                                 "                            }\\r\\n"
                                 "                            setPickerColor(confettiSelectButton, newRed, newGreen, newBlue);\\r\\n"
                                 "                        }\\r\\n"
                                 "                    }\\r\\n"
                                 "                }\\r\\n"
                                 "\\r\\n"
                                 "                function onConfettiButtonEvent(red, green, blue) {\\r\\n"
                                 "                    if (currentRed != red || currentGreen != green || currentBlue != blue) {\\r\\n"
                                 "                        currentRed = red\\r\\n"
                                 "                        currentGreen = green\\r\\n"
                                 "                        currentBlue = blue\\r\\n"
                                 "\\r\\n"
                                 "                        msg = {\\r\\n"
                                 "                            \\\"State\\\": true,\\r\\n"
                                 "                            \\\"Mode\\\": \\\"Confetti\\\",\\r\\n"
                                 "                            \\\"Confetti\\\": {\\r\\n"
                                 "                                \\\"Red\\\": red,\\r\\n"
                                 "                                \\\"Green\\\": green,\\r\\n"
                                 "                                \\\"Blue\\\": blue\\r\\n"
                                 "                            }\\r\\n"
                                 "                        }\\r\\n"
                                 "                        if (Date.now() - confettiDebunce > 50) {\\r\\n"
                                 "                            confettiDebunce = Date.now()\\r\\n"
                                 "                            sendMessage(msg)\\r\\n"
                                 "                        }\\r\\n"
                                 "                    }\\r\\n"
                                 "                    if (msg != confettiLastMessage && Date.now() - confettiDebunce > 50) {\\r\\n"
                                 "                        confettiDebunce = Date.now()\\r\\n"
                                 "                        confettiLastMessage = msg\\r\\n"
                                 "                        sendMessage(msg)\\r\\n"
                                 "                    }\\r\\n"
                                 "                }\\r\\n");
  }
};
