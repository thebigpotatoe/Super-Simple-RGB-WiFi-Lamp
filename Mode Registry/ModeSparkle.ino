class ModeSparkle : public ModeBase
{

private:
    int sparkleSpeed = 30;
    bool sparkleActive = true;
    int sparkleRed = 128;
    int sparkleGreen = 128;
    int sparkleBlue = 128;
    int sparklePixel = random(NUM_LEDS);

public:
    ModeSparkle() {}
    virtual void render()
    {
        EVERY_N_MILLISECONDS(sparkleSpeed)
        {
            if (sparkleActive)
            {
                sparklePixel = random(NUM_LEDS);
                ledString[sparklePixel] = CRGB(sparkleRed, sparkleGreen, sparkleBlue);
            }
            else
            {
                ledString[sparklePixel] = CRGB(0, 0, 0);
            }
            sparkleActive = !sparkleActive;
        }
    }

    virtual void applyConfig(JsonVariant &settings)
    {
        settings["Red"] = sparkleRed = settings["Red"] | sparkleRed;
        settings["Green"] = sparkleGreen = settings["Green"] | sparkleGreen;
        settings["Blue"] = sparkleBlue = settings["Blue"] | sparkleBlue;
        settings["Speed"] = sparkleSpeed = settings["Speed"] | sparkleSpeed;
    }

    virtual void sendWebsiteData(WebSocketsServer &_webSocketServer)
    {
        const char *modeName = "Sparkle";
        const char *tabHtml = PSTR("<h2>Sparkle Mode<\\/h2>\\r\\n"
                                   "<p>This is the Sparkle mode.<\\/p>\\r\\n"
                                   "<div class=\\\"row my-3\\\">\\r\\n"
                                   "    <input id=\\\"sparkleSelectButton\\\" class=\\\"color col mb-2 mx-2 btn btn-lg btn-outline-light\\\" value=\\\"rgb(0,0,0)\\\"><\\/input>\\r\\n"
                                   "<\\/div>\\r\\n");
        const char *tabScript = PSTR("var sparkleLastMessage = \\\"\\\"\\r\\n"
                                     "var sparkleRed = 0;\\r\\n"
                                     "var sparkleGreen = 0;\\r\\n"
                                     "var sparkleBlue = 0;\\r\\n"
                                     "var sparkleDebunce = Date.now()\\r\\n"
                                     "\\r\\n"
                                     "messageEventList.push(handleSparkleMessage)\\r\\n"
                                     "\\r\\n"
                                     "var sparkleSelectButton = $('#sparkleSelectButton').colorPicker({\\r\\n"
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
                                     "    cssAddon: '.cp-xy-slider {width:200px; height:200px;}' +\\r\\n"
                                     "        '.cp-xy-cursor {width:16px; height:16px; border-width:2px; margin:-8px}' +\\r\\n"
                                     "        '.cp-z-slider {height:200px; width:40px;}' +\\r\\n"
                                     "        '.cp-z-cursor {border-width:8px; margin-top:-8px;}' +\\r\\n"
                                     "        '.cp-alpha {height:40px;}' +\\r\\n"
                                     "        '.cp-alpha-cursor {border-width:8px; margin-left:-8px;}',\\r\\n"
                                     "\\r\\n"
                                     "    renderCallback: function ($elm, toggled) {\\r\\n"
                                     "        var colors = this.color.colors;\\r\\n"
                                     "        onSparkleButtonEvent(Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                     "        setPickerColor(sparkleSelectButton, Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                     "    }\\r\\n"
                                     "})\\r\\n"
                                     "$(\\\"#sparkleSpeed\\\").on(\\\"input\\\", function () {\\r\\n"
                                     "    onSparkleEvent()\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#sparkleSpeed\\\").on(\\\"change\\\", function () {\\r\\n"
                                     "    onSparkleEvent()\\r\\n"
                                     "});\\r\\n"
                                     "\\r\\n"
                                     "\\r\\n"
                                     "function handleSparkleMessage(jsonMessage) {\\r\\n"
                                     "    if (\\\"Sparkle\\\" in jsonMessage) {\\r\\n"
                                     "        jsonMessage = jsonMessage.Sparkle\\r\\n"
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
                                     "            setPickerColor(sparkleSelectButton, newRed, newGreen, newBlue);\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n"
                                     "\\r\\n"
                                     "function onSparkleEvent() {\\r\\n"
                                     "    let currentSpeedValue = parseFloat($(\\\"#sparkleSpeed\\\").val())\\r\\n"
                                     "\\r\\n"
                                     "    $(\\\"#sparkleSpeedLabel\\\").html(currentSpeedValue)\\r\\n"
                                     "\\r\\n"
                                     "    msg = {\\r\\n"
                                     "        \\\"State\\\": true,\\r\\n"
                                     "        \\\"Mode\\\": \\\"Sparkle\\\",\\r\\n"
                                     "        \\\"Sparkle\\\": {\\r\\n"
                                     "            \\\"Speed\\\": currentSpeedValue,\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "\\r\\n"
                                     "    if (msg != sparkleLastMessage && Date.now() - sparkleDebunce > 50) {\\r\\n"
                                     "        sparkleDebunce = Date.now()\\r\\n"
                                     "        sparkleLastMessage = msg\\r\\n"
                                     "        sendMessage(msg)\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n"
                                     "\\r\\n"
                                     "function onSparkleButtonEvent(red, green, blue) {\\r\\n"
                                     "    if (currentRed != red || currentGreen != green || currentBlue != blue) {\\r\\n"
                                     "        currentRed = red\\r\\n"
                                     "        currentGreen = green\\r\\n"
                                     "        currentBlue = blue\\r\\n"
                                     "\\r\\n"
                                     "        msg = {\\r\\n"
                                     "            \\\"State\\\": true,\\r\\n"
                                     "            \\\"Mode\\\": \\\"Sparkle\\\",\\r\\n"
                                     "            \\\"Sparkle\\\": {\\r\\n"
                                     "                \\\"Red\\\": red,\\r\\n"
                                     "                \\\"Green\\\": green,\\r\\n"
                                     "                \\\"Blue\\\": blue\\r\\n"
                                     "            }\\r\\n"
                                     "        }\\r\\n"
                                     "        if (Date.now() - sparkleDebunce > 50) {\\r\\n"
                                     "            sparkleDebunce = Date.now()\\r\\n"
                                     "            sendMessage(msg)\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "    if (msg != sparkleLastMessage && Date.now() - sparkleDebunce > 50) {\\r\\n"
                                     "        sparkleDebunce = Date.now()\\r\\n"
                                     "        sparkleLastMessage = msg\\r\\n"
                                     "        sendMessage(msg)\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n");

        String htmlJSON = String("{\"Tab\" : {") + "\"Name\": \"" + modeName + "\", \"tabHtml\" : \"" + tabHtml + "\", \"tabScript\" : \"" + tabScript + "\"}}";

        _webSocketServer.broadcastTXT(htmlJSON.c_str());
    }
};
