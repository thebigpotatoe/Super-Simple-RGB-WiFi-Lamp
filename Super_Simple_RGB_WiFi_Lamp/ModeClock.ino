class ModeClock : public ModeBase
{
private:
    int clockHourRed = 128;
    int clockHourGreen = 128;
    int clockHourBlue = 128;
    int clockMinRed = 128;
    int clockMinGreen = 128;
    int clockMinBlue = 128;
    int clockOnPauseBrightness = 255;
    unsigned long lastClockExecution = 0;

public:
    ModeClock() {}
    virtual void render()
    {
        if (ntpTimeSet)
        {
            // Get the number of seconds between each LED
            int hourLedDeltaT = 43200 / (topNumLeds);
            int minuteLedDeltaT = 3600 / (bottomNumLeds);

            // Get the current time modulated to hours and mins
            int currentHour = now() % 43200;
            int currentMinute = now() % 3600;

            // Get the current percentage the time is between 2 LEDS
            int hourGapTime = currentHour % hourLedDeltaT;
            int minuteGapTime = currentMinute % minuteLedDeltaT;
            float hourPercentOfGap = (float)hourGapTime / (float)hourLedDeltaT;
            float minutePercentOfGap = (float)minuteGapTime / (float)minuteLedDeltaT;

            // Calculate the current and next LED to turn on
            int hourLEDNumber = floor(currentHour / hourLedDeltaT);
            int hourCurrentLED = topLeds[hourLEDNumber];
            int hourNextLED = (hourLEDNumber == topNumLeds - 1) ? topLeds[0] : topLeds[hourLEDNumber + 1];
            int minuteLEDNumber = floor(currentMinute / minuteLedDeltaT);
            int minuteCurrentLED = bottomLeds[minuteLEDNumber];
            int minuteNextLED = (minuteLEDNumber == bottomNumLeds - 1) ? bottomLeds[0] : bottomLeds[minuteLEDNumber + 1];

            // Calculate the brightness of the current and next LED based on the percentage
            int hourCurrentLEDBrightness = 255 * (1 - hourPercentOfGap);
            int hourNextLEDBrightness = 255 * (hourPercentOfGap);
            int minuteCurrentLEDBrightness = 255 * (1 - minutePercentOfGap);
            int minuteNextLEDBrightness = 255 * (minutePercentOfGap);

            // Clear all the LED's
            FastLED.clear();

            // Set the colour of the LED
            ledString[hourCurrentLED] = CRGB(clockHourRed, clockHourGreen, clockHourBlue);
            ledString[hourNextLED] = ledString[hourCurrentLED];
            ledString[minuteCurrentLED] = CRGB(clockMinRed, clockMinGreen, clockMinBlue);
            ledString[minuteNextLED] = ledString[minuteCurrentLED];

            // Dim the led correctly
            ledString[hourCurrentLED].nscale8(hourCurrentLEDBrightness);
            ledString[hourNextLED].nscale8(hourNextLEDBrightness);
            ledString[minuteCurrentLED].nscale8(minuteCurrentLEDBrightness);
            ledString[minuteNextLED].nscale8(minuteNextLEDBrightness);
        }
        else
        {
            // Set each of the lights colours
            for (int i = 0; i < topNumLeds; i++)
            {
                ledString[topLeds[i]] = CRGB(clockHourRed, clockHourGreen, clockHourBlue);
            }
            for (int i = 0; i < topNumLeds; i++)
            {
                ledString[bottomLeds[i]] = CRGB(clockMinRed, clockMinGreen, clockMinBlue);
            }

            // Set the brightness up and down
            // Serial.println(sin8(clockOnPauseBrightness));
            nscale8(ledString, NUM_LEDS, triwave8(clockOnPauseBrightness));
            clockOnPauseBrightness += 1;
        }
    }

    virtual void applyConfig(JsonVariant &settings)
    {
        if (settings.containsKey("Epoch"))
        {
            settings["Epoch"] = currentEpochTime = settings["Epoch"] | currentEpochTime;
            setTime(currentEpochTime);
        }

        JsonVariant hourColourSettings = settings["hourColour"];
        if (hourColourSettings)
        {
            hourColourSettings["Red"] = clockHourRed = hourColourSettings["Red"] | clockHourRed;
            hourColourSettings["Green"] = clockHourGreen = hourColourSettings["Green"] | clockHourGreen;
            hourColourSettings["Blue"] = clockHourBlue = hourColourSettings["Blue"] | clockHourBlue;
        }

        JsonVariant minColourSettings = settings["minColour"];
        if (minColourSettings)
        {
            minColourSettings["Red"] = clockMinRed = minColourSettings["Red"] | clockMinRed;
            minColourSettings["Green"] = clockMinGreen = minColourSettings["Green"] | clockMinGreen;
            minColourSettings["Blue"] = clockMinBlue = minColourSettings["Blue"] | clockMinBlue;
        }
    }

    virtual void sendWebsiteData(WebSocketsServer &_webSocketServer)
    {
        const char *modeName = "Clock";
        const char *tabHtml = PSTR("<h2>Clock Mode<\\/h2>\\r\\n"
                                   "<p>In this mode the light will display the current time in 12 hr format uisng the\\r\\n"
                                   "    top and bottom side of the light. On the top is the current hour, and the bottom is the minute. The left\\r\\n"
                                   "    of teh light represents 0 and the right represents either 12hr or 60mins. You can choose the colour of\\r\\n"
                                   "    the hour and minute light to what you would desire. If your clock is out of sync\\r\\n"
                                   "    you can click the resync button (note this should be automatically done on the device for you)<\\/p>\\r\\n"
                                   "<h2>Current Time: <span id=\\\"clockPrintOut\\\"><\\/span><\\/h2>\\r\\n"
                                   "<div class=\\\"row my-3\\\">\\r\\n"
                                   "    <button id=\\\"clockHourColourButton\\\" class=\\\"color col mb-2 mx-2 btn btn-lg btn-outline-light\\\">Hour Colour<\\/button>\\r\\n"
                                   "    <button id=\\\"clockMinuteColourButton\\\" class=\\\"color col mb-2 mx-2 btn btn-lg btn-outline-light\\\">Minute Colour<\\/button>\\r\\n"
                                   "<\\/div>\\r\\n");
        const char *tabScript = PSTR("clockHourDebunce = Date.now()\\r\\n"
                                     "clockMinDebunce = Date.now()\\r\\n"
                                     "var currentHourRed = 0\\r\\n"
                                     "var currentHourGreen = 0\\r\\n"
                                     "var currentHourBlue = 0\\r\\n"
                                     "var currentMinRed = 0\\r\\n"
                                     "var currentMinGreen = 0\\r\\n"
                                     "var currentMinBlue = 0\\r\\n"
                                     "\\r\\n"
                                     "messageEventList.push(handleClockMessage)\\r\\n"
                                     "\\r\\n"
                                     "var clockHourColourButton = $('#clockHourColourButton').colorPicker({\\r\\n"
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
                                     "        onHourPickerEvent(Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                     "        setPickerColor(clockHourColourButton, Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                     "    }\\r\\n"
                                     "})\\r\\n"
                                     "var clockMinuteColourButton = $('#clockMinuteColourButton').colorPicker({\\r\\n"
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
                                     "        onMinPickerEvent(Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                     "        setPickerColor(clockMinuteColourButton, Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                     "    }\\r\\n"
                                     "})\\r\\n"
                                     "\\r\\n"
                                     "$(function () {\\r\\n"
                                     "    clockTick()\\r\\n"
                                     "});\\r\\n"
                                     "\\r\\n"
                                     "function clockTick() {\\r\\n"
                                     "    let today = new Date();\\r\\n"
                                     "    let h = (today.getHours() % 12 < 10) ? \\\"0\\\" + today.getHours() % 12 : today.getHours() % 12\\r\\n"
                                     "    let m = (today.getMinutes() < 10) ? \\\"0\\\" + today.getMinutes() : today.getMinutes()\\r\\n"
                                     "    let s = (today.getSeconds() < 10) ? \\\"0\\\" + today.getSeconds() : today.getSeconds()\\r\\n"
                                     "    let ampm = (today.getHours() >= 12) ? 'PM' : 'AM';\\r\\n"
                                     "    h = (today.getHours() != 0) ? h : \\\"12\\\";\\r\\n"
                                     "\\r\\n"
                                     "    var t = setTimeout(clockTick, 1000);\\r\\n"
                                     "    $(\\\"#clockPrintOut\\\").html(h + \\\":\\\" + m + \\\":\\\" + s + \\\" \\\" + ampm)\\r\\n"
                                     "}\\r\\n"
                                     "\\r\\n"
                                     "function handleClockMessage(jsonMessage) {\\r\\n"
                                     "    if (\\\"Clock\\\" in jsonMessage) {\\r\\n"
                                     "        jsonMessage = jsonMessage.Clock\\r\\n"
                                     "        if (typeof jsonMessage === \\\"object\\\") {\\r\\n"
                                     "            if ((\\\"hourColour\\\" in jsonMessage)) {\\r\\n"
                                     "                if (typeof jsonMessage.hourColour === \\\"object\\\") {\\r\\n"
                                     "                    var newRed = currentHourRed\\r\\n"
                                     "                    var newGreen = currentHourGreen\\r\\n"
                                     "                    var newBlue = currentHourBlue\\r\\n"
                                     "\\r\\n"
                                     "                    if ((\\\"Red\\\" in jsonMessage.hourColour)) {\\r\\n"
                                     "                        if (typeof jsonMessage.hourColour.Red === \\\"number\\\") {\\r\\n"
                                     "                            newRed = jsonMessage.hourColour.Red\\r\\n"
                                     "                        }\\r\\n"
                                     "                    }\\r\\n"
                                     "                    if ((\\\"Green\\\" in jsonMessage.hourColour)) {\\r\\n"
                                     "                        if (typeof jsonMessage.hourColour.Green === \\\"number\\\") {\\r\\n"
                                     "                            newGreen = jsonMessage.hourColour.Green\\r\\n"
                                     "                        }\\r\\n"
                                     "                    }\\r\\n"
                                     "                    if ((\\\"Blue\\\" in jsonMessage.hourColour)) {\\r\\n"
                                     "                        if (typeof jsonMessage.hourColour.Blue === \\\"number\\\") {\\r\\n"
                                     "                            newBlue = jsonMessage.hourColour.Blue\\r\\n"
                                     "                        }\\r\\n"
                                     "                    }\\r\\n"
                                     "\\r\\n"
                                     "                    setPickerColor(clockHourColourButton, newRed, newGreen, newBlue);\\r\\n"
                                     "                }\\r\\n"
                                     "            }\\r\\n"
                                     "\\r\\n"
                                     "            if ((\\\"minuteColour\\\" in jsonMessage)) {\\r\\n"
                                     "                if (typeof jsonMessage.minuteColour === \\\"object\\\") {\\r\\n"
                                     "                    var newRed = currentMinRed\\r\\n"
                                     "                    var newGreen = currentMinGreen\\r\\n"
                                     "                    var newBlue = currentMinBlue\\r\\n"
                                     "\\r\\n"
                                     "                    if ((\\\"Red\\\" in jsonMessage.minuteColour)) {\\r\\n"
                                     "                        if (typeof jsonMessage.minuteColour.Red === \\\"number\\\") {\\r\\n"
                                     "                            newRed = jsonMessage.minuteColour.Red\\r\\n"
                                     "                        }\\r\\n"
                                     "                    }\\r\\n"
                                     "                    if ((\\\"Green\\\" in jsonMessage.minuteColour)) {\\r\\n"
                                     "                        if (typeof jsonMessage.minuteColour.Green === \\\"number\\\") {\\r\\n"
                                     "                            newGreen = jsonMessage.minuteColour.Green\\r\\n"
                                     "                        }\\r\\n"
                                     "                    }\\r\\n"
                                     "                    if ((\\\"Blue\\\" in jsonMessage.minuteColour)) {\\r\\n"
                                     "                        if (typeof jsonMessage.minuteColour.Blue === \\\"number\\\") {\\r\\n"
                                     "                            newBlue = jsonMessage.minuteColour.Blue\\r\\n"
                                     "                        }\\r\\n"
                                     "                    }\\r\\n"
                                     "\\r\\n"
                                     "                    setPickerColor(clockMinuteColourButton, newRed, newGreen, newBlue);\\r\\n"
                                     "                }\\r\\n"
                                     "            }\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n"
                                     "\\r\\n"
                                     "function onHourPickerEvent(red, green, blue) {\\r\\n"
                                     "    if (currentHourRed != red || currentHourGreen != green || currentHourBlue != blue) {\\r\\n"
                                     "        currentHourRed = red\\r\\n"
                                     "        currentHourGreen = green\\r\\n"
                                     "        currentHourBlue = blue\\r\\n"
                                     "\\r\\n"
                                     "        msg = {\\r\\n"
                                     "            \\\"State\\\": true,\\r\\n"
                                     "            \\\"Mode\\\": \\\"Clock\\\",\\r\\n"
                                     "            \\\"Clock\\\": {\\r\\n"
                                     "                \\\"hourColour\\\": {\\r\\n"
                                     "                    \\\"Red\\\": red,\\r\\n"
                                     "                    \\\"Green\\\": green,\\r\\n"
                                     "                    \\\"Blue\\\": blue\\r\\n"
                                     "                }\\r\\n"
                                     "            }\\r\\n"
                                     "        }\\r\\n"
                                     "\\r\\n"
                                     "        if (Date.now() - clockHourDebunce > 50) {\\r\\n"
                                     "            clockHourDebunce = Date.now()\\r\\n"
                                     "            sendMessage(msg)\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n"
                                     "\\r\\n"
                                     "function onMinPickerEvent(red, green, blue) {\\r\\n"
                                     "    if (currentMinRed != red || currentMinGreen != green || currentMinBlue != blue) {\\r\\n"
                                     "        currentMinRed = red\\r\\n"
                                     "        currentMinGreen = green\\r\\n"
                                     "        currentMinBlue = blue\\r\\n"
                                     "\\r\\n"
                                     "        msg = {\\r\\n"
                                     "            \\\"State\\\": true,\\r\\n"
                                     "            \\\"Mode\\\": \\\"Clock\\\",\\r\\n"
                                     "            \\\"Clock\\\": {\\r\\n"
                                     "                \\\"minColour\\\": {\\r\\n"
                                     "                    \\\"Red\\\": red,\\r\\n"
                                     "                    \\\"Green\\\": green,\\r\\n"
                                     "                    \\\"Blue\\\": blue\\r\\n"
                                     "                }\\r\\n"
                                     "            }\\r\\n"
                                     "        }\\r\\n"
                                     "\\r\\n"
                                     "        if (Date.now() - clockMinDebunce > 50) {\\r\\n"
                                     "            clockMinDebunce = Date.now()\\r\\n"
                                     "            sendMessage(msg)\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n");
        String htmlJSON = String("{\"Tab\" : {") + "\"Name\": \"" + modeName + "\", \"tabHtml\" : \"" + tabHtml + "\", \"tabScript\" : \"" + tabScript + "\"}}";

        _webSocketServer.broadcastTXT(htmlJSON.c_str());
    }
};
