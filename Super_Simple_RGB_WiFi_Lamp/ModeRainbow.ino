class ModeRainbow : public ModeBase
{
private:
  int rainbowStartHue = 0;
  int rainbowSpeed = 10;
  int rainbowBri = 100;
  float rainbowAddedHue = 0;

public:
  ModeRainbow() {}
  virtual void render()
  {
    int startHue = rainbowStartHue;
    int speed = rainbowSpeed;
    int brightness = rainbowBri;

    // Constrain the variables before using
    startHue = constrain(startHue, 0, 255);
    speed = speed > 0 ? speed : 0;
    brightness = constrain(brightness, 0, 255);

    // Update the hue by 1 every 360th of the allocated time
    if (speed > 0)
    {
      float rainbowDeltaHue = (255 / ((float)speed * 1000)) * 50;
      EVERY_N_MILLISECONDS(50)
      {
        rainbowAddedHue += rainbowDeltaHue;
        rainbowAddedHue = (rainbowAddedHue > 255) ? rainbowAddedHue - 255 : rainbowAddedHue;
      };

      startHue += (int)rainbowAddedHue;
    }

    // Calculate the rainbow so it lines up
    float deltaHue = (float)255 / (float)NUM_LEDS;
    float currentHue = startHue;
    for (int i = 0; i < NUM_LEDS; i++)
    {
      currentHue = startHue + (float)(deltaHue * i);
      currentHue = (currentHue < 255) ? currentHue : currentHue - 255;
      ledString[i] = CHSV(currentHue, 255, 255);
    }

    FastLED.setBrightness(brightness);
  }

  virtual void applyConfig(JsonVariant &settings)
  {
    settings["Hue"] = rainbowStartHue = settings["Hue"] | rainbowStartHue;
    settings["Speed"] = rainbowSpeed = settings["Speed"] | rainbowSpeed;
    settings["Brightness"] = rainbowBri = settings["Brightness"] | rainbowBri;
  }

  virtual void sendWebsiteData(WebSocketsServer &_webSocketServer)
  {
    const char *modeName = "Rainbow";
    const char *tabHtml = PSTR("<h2>Rainbow Mode<\\/h2>\\r\\n"
                               "<p>Here you can set the mode to rainbow. This mode produces a rainbow all the way around the light and\\r\\n"
                               "    slowly shifts the colours clockwise. On this page you can set the speed of this as well as the\\r\\n"
                               "    brightness of the light<\\/p>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"rainbowHue\\\">Start Hue: <span id=\\\"rainbowHueLabel\\\">0<\\/span> Degrees<\\/label>\\r\\n"
                               "    <input id=\\\"rainbowHue\\\" type=\\\"range\\\" min=\\\"0\\\" max=\\\"359\\\" step=\\\"1\\\" value=\\\"0\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"rainbowBrightness\\\">Rainbow Brightness: <span id=\\\"rainbowBrightnessLabel\\\">100<\\/span>%<\\/label>\\r\\n"
                               "    <input id=\\\"rainbowBrightness\\\" type=\\\"range\\\" min=\\\"0\\\" max=\\\"100\\\" step=\\\"1\\\" value=\\\"100\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"rainbowSpeed\\\">Rainbow Speed: <span id=\\\"rainbowSpeedLabel\\\">10<\\/span> seconds<\\/label>\\r\\n"
                               "    <input id=\\\"rainbowSpeed\\\" type=\\\"range\\\" min=\\\"0\\\" max=\\\"10\\\" step=\\\"1\\\" value=\\\"10\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n");

    const char *tabScript = PSTR("var rainbowDebunce = Date.now()\\r\\n"
                                 "var rainbowLastMessage = \\\"\\\"\\r\\n"
                                 "\\r\\n"
                                 "messageEventList.push(handleRainbowMessage)\\r\\n"
                                 "\\r\\n"
                                 "$(\\\"#rainbowHue\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onRainboWHueEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#rainbowHue\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onRainboWHueEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#rainbowSpeed\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onRainboWEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#rainbowSpeed\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onRainboWEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#rainbowBrightness\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onRainboWEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#rainbowBrightness\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onRainboWEvent()\\r\\n"
                                 "});\\r\\n"
                                 "\\r\\n"
                                 "function handleRainbowMessage(jsonMessage) {\\r\\n"
                                 "    if (\\\"Rainbow\\\" in jsonMessage) {\\r\\n"
                                 "        jsonMessage = jsonMessage.Rainbow\\r\\n"
                                 "        if (typeof jsonMessage === \\\"object\\\") {\\r\\n"
                                 "            if ((\\\"Hue\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.Hue === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#rainbowHue\\\").val(Math.round(jsonMessage.Hue / 255 * 360))\\r\\n"
                                 "                    $(\\\"#rainbowHueLabel\\\").html(Math.round(jsonMessage.Hue / 255 * 360))\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"Speed\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.Speed === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#rainbowSpeed\\\").val(jsonMessage.Speed)\\r\\n"
                                 "                    $(\\\"#rainbowSpeedLabel\\\").html(jsonMessage.Speed)\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"Brightness\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.Brightness === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#rainbowBrightness\\\").val(Math.round(jsonMessage.Brightness / 255 * 100))\\r\\n"
                                 "                    $(\\\"#rainbowBrightnessLabel\\\").html(Math.round(jsonMessage.Brightness / 255 * 100))\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n"
                                 "\\r\\n"
                                 "function onRainboWHueEvent() {\\r\\n"
                                 "\\r\\n"
                                 "    let currentHueValue = parseInt($(\\\"#rainbowHue\\\").val(), 10)\\r\\n"
                                 "    let currentBrightnessValue = parseInt($(\\\"#rainbowBrightness\\\").val(), 10)\\r\\n"
                                 "\\r\\n"
                                 "    $(\\\"#rainbowSpeed\\\").val = 0;\\r\\n"
                                 "    $(\\\"#rainbowSpeedLabel\\\").html(0)\\r\\n"
                                 "    $(\\\"#rainbowHueLabel\\\").html(currentHueValue)\\r\\n"
                                 "    $(\\\"#rainbowBrightnessLabel\\\").html(currentBrightnessValue)\\r\\n"
                                 "\\r\\n"
                                 "    msg = {\\r\\n"
                                 "        \\\"State\\\": true,\\r\\n"
                                 "        \\\"Mode\\\": \\\"Rainbow\\\",\\r\\n"
                                 "        \\\"Rainbow\\\": {\\r\\n"
                                 "            \\\"Hue\\\": Math.round(currentHueValue / 360 * 255),\\r\\n"
                                 "            \\\"Speed\\\": 0,\\r\\n"
                                 "            \\\"Brightness\\\": Math.round(currentBrightnessValue / 100 * 255)\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "\\r\\n"
                                 "    if (msg != rainbowLastMessage && Date.now() - rainbowDebunce > 50) {\\r\\n"
                                 "        rainbowDebunce = Date.now()\\r\\n"
                                 "        rainbowLastMessage = msg\\r\\n"
                                 "        sendMessage(msg)\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n"
                                 "\\r\\n"
                                 "function onRainboWEvent() {\\r\\n"
                                 "    let currentSpeedValue = parseFloat($(\\\"#rainbowSpeed\\\").val())\\r\\n"
                                 "    let currentBrightnessValue = parseInt($(\\\"#rainbowBrightness\\\").val(), 10)\\r\\n"
                                 "\\r\\n"
                                 "    $(\\\"#rainbowSpeedLabel\\\").html(currentSpeedValue)\\r\\n"
                                 "    $(\\\"#rainbowBrightnessLabel\\\").html(currentBrightnessValue)\\r\\n"
                                 "\\r\\n"
                                 "    msg = {\\r\\n"
                                 "        \\\"State\\\": true,\\r\\n"
                                 "        \\\"Mode\\\": \\\"Rainbow\\\",\\r\\n"
                                 "        \\\"Rainbow\\\": {\\r\\n"
                                 "            \\\"Speed\\\": currentSpeedValue,\\r\\n"
                                 "            \\\"Brightness\\\": Math.round(currentBrightnessValue / 100 * 255)\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "\\r\\n"
                                 "    if (msg != rainbowLastMessage && Date.now() - rainbowDebunce > 50) {\\r\\n"
                                 "        rainbowDebunce = Date.now()\\r\\n"
                                 "        rainbowLastMessage = msg\\r\\n"
                                 "        sendMessage(msg)\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n");
    String htmlJSON = String("{\"Tab\" : {") + "\"Name\": \"" + modeName + "\", \"tabHtml\" : \"" + tabHtml + "\", \"tabScript\" : \"" + tabScript + "\"}}";

    _webSocketServer.broadcastTXT(htmlJSON.c_str());
  }
};
