class ModeColour : public ModeBase
{
private:
    int colourRed = 128;
    int colourGreen = 128;
    int colourBlue = 128;

public:
    ModeColour() {}
    virtual void render()
    {
        fill_solid(ledString, NUM_LEDS, CRGB(colourRed, colourGreen, colourBlue));
    }

    virtual void applyConfig(JsonVariant &settings)
    {
        settings["Red"] = colourRed = settings["Red"] | colourRed;
        settings["Green"] = colourGreen = settings["Green"] | colourGreen;
        settings["Blue"] = colourBlue = settings["Blue"] | colourBlue;
    }

    virtual void sendWebsiteData(WebSocketsServer &_webSocketServer)
    {
        const char *modeName = "Colour";
        const char *tabHtml = PSTR("<h2>Colour Mode<\\/h2>\\r\\n"
                                   "<p>Here you can set the light to any colour you desire. There are also a couple of buttons for setting the light to different shades of white<\\/p>\\r\\n"
                                   "\\r\\n"
                                   "<div class=\\\"row my-3\\\">\\r\\n"
                                   "    <input id=\\\"colourSelectButton\\\" class=\\\"color col mb-2 mx-2 btn btn-lg btn-outline-light\\\" value=\\\"rgb(0,0,0)\\\"><\\/input>\\r\\n"
                                   "<\\/div>\\r\\n"
                                   "<div class=\\\"row my-3\\\">\\r\\n"
                                   "    <button id=\\\"redButton\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">Red<\\/button>\\r\\n"
                                   "    <button id=\\\"greenButton\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">Green<\\/button>\\r\\n"
                                   "    <button id=\\\"blueButton\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">Blue<\\/button>\\r\\n"
                                   "<\\/div>\\r\\n"
                                   "<div class=\\\"row my-3\\\">\\r\\n"
                                   "    <button id=\\\"yellowButton\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">Yellow<\\/button>\\r\\n"
                                   "    <button id=\\\"cyanButton\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">Cyan<\\/button>\\r\\n"
                                   "    <button id=\\\"magentaButton\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">Magenta<\\/button>\\r\\n"
                                   "<\\/div>\\r\\n"
                                   "<div class=\\\"row my-3\\\">\\r\\n"
                                   "    <button id=\\\"2500Button\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">2500K<\\/button>\\r\\n"
                                   "    <button id=\\\"3000Button\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">3000K<\\/button>\\r\\n"
                                   "    <button id=\\\"4000Button\\\" class=\\\"col mb-2 mx-2 btn btn-lg btn-outline-light\\\">4000K<\\/button>\\r\\n"
                                   "<\\/div>\\r\\n");
        const char *tabScript = PSTR("var currentRed = 0;\\r\\n"
                                     "var currentGreen = 0;\\r\\n"
                                     "var currentBlue = 0;\\r\\n"
                                     "var colourDebunce = Date.now()\\r\\n"
                                     "\\r\\n"
                                     "messageEventList.push(handleColourMessage)\\r\\n"
                                     "\\r\\n"
                                     "var colourSelectButton = $('#colourSelectButton').colorPicker({\\r\\n"
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
                                     "    cssAddon:\\r\\n"
                                     "        '.cp-xy-slider {width:200px; height:200px;}' +\\r\\n"
                                     "        '.cp-xy-cursor {width:16px; height:16px; border-width:2px; margin:-8px}' +\\r\\n"
                                     "        '.cp-z-slider {height:200px; width:40px;}' +\\r\\n"
                                     "        '.cp-z-cursor {border-width:8px; margin-top:-8px;}' +\\r\\n"
                                     "        '.cp-alpha {height:40px;}' +\\r\\n"
                                     "        '.cp-alpha-cursor {border-width:8px; margin-left:-8px;}',\\r\\n"
                                     "\\r\\n"
                                     "    renderCallback: function ($elm, toggled) {\\r\\n"
                                     "        var colors = this.color.colors;\\r\\n"
                                     "        onColourButtonEvent(Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                     "        setPickerColor(colourSelectButton, Math.round(colors.rgb.r * 255), Math.round(colors.rgb.g * 255), Math.round(colors.rgb.b * 255))\\r\\n"
                                     "    }\\r\\n"
                                     "})\\r\\n"
                                     "$(\\\"#redButton\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(255, 0, 0)\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#greenButton\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(0, 255, 0)\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#blueButton\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(0, 0, 255)\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#yellowButton\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(255, 255, 0)\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#cyanButton\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(0, 255, 255)\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#magentaButton\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(255, 0, 255)\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#2500Button\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(206, 57, 18)\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#3000Button\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(235, 71, 30)\\r\\n"
                                     "});\\r\\n"
                                     "$(\\\"#4000Button\\\").click(function () {\\r\\n"
                                     "    onColourButtonEvent(238, 99, 63)\\r\\n"
                                     "});\\r\\n"
                                     "\\r\\n"
                                     "function handleColourMessage(jsonMessage) {\\r\\n"
                                     "    if (\\\"Colour\\\" in jsonMessage) {\\r\\n"
                                     "        jsonMessage = jsonMessage.Colour\\r\\n"
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
                                     "\\r\\n"
                                     "            setPickerColor(colourSelectButton, newRed, newGreen, newBlue);\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n"
                                     "\\r\\n"
                                     "function onColourButtonEvent(red, green, blue) {\\r\\n"
                                     "    if (currentRed != red || currentGreen != green || currentBlue != blue) {\\r\\n"
                                     "        currentRed = red\\r\\n"
                                     "        currentGreen = green\\r\\n"
                                     "        currentBlue = blue\\r\\n"
                                     "\\r\\n"
                                     "        msg = {\\r\\n"
                                     "            \\\"State\\\": true,\\r\\n"
                                     "            \\\"Mode\\\": \\\"Colour\\\",\\r\\n"
                                     "            \\\"Colour\\\": {\\r\\n"
                                     "                \\\"Red\\\": red,\\r\\n"
                                     "                \\\"Green\\\": green,\\r\\n"
                                     "                \\\"Blue\\\": blue\\r\\n"
                                     "            }\\r\\n"
                                     "        }\\r\\n"
                                     "        if (Date.now() - colourDebunce > 50) {\\r\\n"
                                     "            colourDebunce = Date.now()\\r\\n"
                                     "            sendMessage(msg)\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n");

        String htmlJSON = String("{\"Tab\" : {") + "\"Name\": \"" + modeName + "\", \"tabHtml\" : \"" + tabHtml + "\", \"tabScript\" : \"" + tabScript + "\"}}";

        _webSocketServer.broadcastTXT(htmlJSON.c_str());
    }
};
