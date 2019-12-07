class ModeCircle : public ModeBase
{
private:
  int circleActiveLedNumber = 0;

public:
  ModeCircle() {}
  virtual void render()
  {
    // First bring our logical arrays into a list of led numbers to iterate over
    int i;
    int ledIter = 0;
    int leds[NUM_LEDS];
    for (i = 0; i < bottomNumLeds; i++)
    {
      leds[ledIter++] = bottomLeds[i];
    }
    for (i = leftNumLeds - 1; i >= 0; i--)
    {
      leds[ledIter++] = leftLeds[i];
    }
    for (i = topNumLeds - 1; i >= 0; i--)
    {
      leds[ledIter++] = topLeds[i];
    }
    for (i = rightNumLeds - 1; i >= 0; i--)
    {
      leds[ledIter++] = rightLeds[i];
    }

    // Update the active LED index
    EVERY_N_MILLISECONDS(40)
    {
      circleActiveLedNumber += 1;
      if (circleActiveLedNumber == NUM_LEDS)
        circleActiveLedNumber = 0;

      // Serial.print("Active number: ");
      // Serial.println(circleActiveLedNumber);

      // Darken all LEDs to slightly dim the previous active LEDs
      fadeToBlackBy(ledString, NUM_LEDS, 80);
    };

    // And now highlight the active index
    for (i = 0; i < NUM_LEDS; i++)
    {
      if (i == circleActiveLedNumber)
      {
        ledString[leds[i]] = CRGB::Red;
      }
    }
  }

  virtual void applyConfig(JsonVariant &settings) {}

  virtual void sendWebsiteData(WebSocketsServer &_webSocketServer)
  {
    const char *modeName = "Circle";
    const char *tabHtml = PSTR("<h2>Circle Mode<\\/h2>"
                               "<p>A simple dot moving round the lamp.<\\/p>");
    const char *tabScript = PSTR("messageEventList.push(handleCircleMessage);\\r\\n"
                                 "function handleCircleMessage(jsonMessage) {\\r\\n"
                                 "     if (\\\"Circle\\\" in jsonMessage) {\\r\\n"
                                 "         jsonMessage = jsonMessage.Circle\\r\\n"
                                 "        if (typeof jsonMessage === \\\"object\\\") {}\\r\\n"
                                 "     }\\r\\n"
                                 " }\\r\\n");
    String htmlJSON = String("{\"Tab\" : {") + "\"Name\": \"" + modeName + "\", \"tabHtml\" : \"" + tabHtml + "\", \"tabScript\" : \"" + tabScript + "\"}}";

    _webSocketServer.broadcastTXT(htmlJSON.c_str());
  }
};