#define VISUALISER_NUM_SAMPLES 64

class ModeVisualiser : public ModeBase
{
private:
  ADC_MODE(ADC_TOUT);
  arduinoFFT FFT = arduinoFFT();
  double visualiserRealSamples[VISUALISER_NUM_SAMPLES];
  double visualiserImaginarySamples[VISUALISER_NUM_SAMPLES];
  unsigned long visualiserLastSampleTime = 0;
  uint16_t visualiserPeriod = 250;
  uint16_t visualiserMinThreshold = 100;
  uint16_t visualiserMaxThreshold = 750;
  uint8_t visualiserNumBinsToSkip = 3;
  uint8_t visualiserFadeUp = 32;
  uint8_t visualiserFadeDown = 32;
  uint8_t visualiserHueOffset = 170;

public:
  ModeVisualiser() {}
  virtual void render()
  {
    // Only use visualiser when not trying to access the NTP server
    if (((WiFi.isConnected() && ntpTimeSet) || softApStarted) && !webSocketConnecting)
    {
      // ************* ADC Reading *************
      // Turn off interupts
      system_soft_wdt_stop();
      ets_intr_lock(); //close interrupt
      noInterrupts();

      // Set up the buffer
      int sampleNumber = 0;
      uint16_t sampleBuffer[1];

      // Read the first value - seems to be invalid on first read?
      system_adc_read_fast(sampleBuffer, 1, 8);

      // Constrain the period
      visualiserPeriod = constrain(visualiserPeriod, 0, 2000);

      // Sample the ADC until buffer is full
      unsigned long adcBufferTime = micros();
      while (sampleNumber < VISUALISER_NUM_SAMPLES)
      {
        // Get the ADC reading
        adcBufferTime = micros();
        system_adc_read_fast(sampleBuffer, 1, 8);

        // If the correct period of time has passed store the reading
        if (sampleNumber < VISUALISER_NUM_SAMPLES && adcBufferTime - visualiserLastSampleTime > visualiserPeriod)
        {
          visualiserRealSamples[sampleNumber] = sampleBuffer[0];
          visualiserImaginarySamples[sampleNumber] = 0.0;
          visualiserLastSampleTime = adcBufferTime;
          sampleNumber++;
        }
      }

      // Turn interupts back on
      interrupts();
      ets_intr_unlock(); //open interrupt
      system_soft_wdt_restart();

      // Debug for ADC - Should be centered around 512
      // for (int i = 0; i < VISUALISER_NUM_SAMPLES; i++) {
      //   Serial.println(visualiserRealSamples[i]);
      // }

      // ************* Calculate FFT *************
      FFT.Windowing(visualiserRealSamples, VISUALISER_NUM_SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(visualiserRealSamples, visualiserImaginarySamples, VISUALISER_NUM_SAMPLES, FFT_FORWARD);
      FFT.ComplexToMagnitude(visualiserRealSamples, visualiserImaginarySamples, VISUALISER_NUM_SAMPLES);

      // ************* Set the LED's *************
      // Set the colour of each light based on the values calculated
      for (int ledNum = 0; ledNum < topNumLeds; ledNum++)
      {
        // Map to the bin number, skip all bins required. Start at the second bin to avoid DC
        uint8_t binNumber = (ledNum < visualiserNumBinsToSkip) ? visualiserNumBinsToSkip : constrain(ledNum, 0, VISUALISER_NUM_SAMPLES / 2);
        // Serial.print(binNumber);
        // Serial.print("\t");

        // Subract the minium value chosen for reduction of artifacts
        double adjustedBinValue = (visualiserRealSamples[binNumber] > visualiserMinThreshold) ? visualiserRealSamples[binNumber] - visualiserMinThreshold : 0.0;
        // Serial.print(adjustedBinValue);
        // Serial.print("\t");

        // Set if visualiserRealSamples[binNumber] is above 0
        if (adjustedBinValue > 0)
        {
          // Map the float values to 8 bit integers
          uint8_t brightnessValue = map(adjustedBinValue, 0, visualiserMaxThreshold, 0, 255);
          // Serial.println(brightnessValue);

          // Get the current hue of the rainbow for the specific LED
          uint8_t ledHue = int(255.0 / (topNumLeds - 1) * ledNum + visualiserHueOffset) % 255;
          CRGB newColour = CRGB(CHSV(ledHue, 255, 255)).nscale8(brightnessValue * (visualiserFadeUp / 255.00));

          // Add the new colour to the current LED
          ledString[topLeds[ledNum]] = ledString[bottomLeds[ledNum]] += newColour;

          // If the LED num is the first or last, use it to set the sides
          if (ledNum == 0)
          {
            for (int sideLedNum = 0; sideLedNum < rightNumLeds; sideLedNum++)
            {
              ledString[rightLeds[sideLedNum]] += newColour;
            }
          }
          else if (ledNum == topNumLeds - 1)
          {
            for (int sideLedNum = 0; sideLedNum < leftNumLeds; sideLedNum++)
            {
              ledString[leftLeds[sideLedNum]] += newColour;
            }
          }
        }
      }

      // Fade all leds gradually for a smooth effect
      fadeToBlackBy(ledString, NUM_LEDS, visualiserFadeDown);
      // fadeLightBy(ledString, NUM_LEDS, visualiserFadeDown);
    }
    else
    {
      // Serial.println("Websockets Connecting");
      delay(1000);
    }
  }

  virtual void applyConfig(JsonVariant &settings)
  {
    settings["Period"] = visualiserPeriod = settings["Period"] | visualiserPeriod;
    settings["MinThreshold"] = visualiserMinThreshold = settings["MinThreshold"] | visualiserMinThreshold;
    settings["MaxThreshold"] = visualiserMaxThreshold = settings["MaxThreshold"] | visualiserMaxThreshold;
    settings["FadeUp"] = visualiserFadeUp = settings["FadeUp"] | visualiserFadeUp;
    settings["FadeDown"] = visualiserFadeDown = settings["FadeDown"] | visualiserFadeDown;
    settings["HueOffset"] = visualiserHueOffset = settings["HueOffset"] | visualiserHueOffset;
  }

  virtual const char *getName()
  {
    return "Visualiser";
  }

  virtual const char *getTabHtml()
  {
    return PSTR("<h2>Visualiser Mode<\\/h2>\\r\\n"
                               "<p> Here you can set the mode to Visualiser. This mode does an FFT on the ADC of the ESP8266 and maps the\\r\\n"
                               "    frequencies\\r\\n"
                               "    to the number of top and bottom LED's. To use this mode, an input source must be present on the ADC such\\r\\n"
                               "    as an amplified mic\\r\\n"
                               "    or an input from a music source such as a Chromecast.\\r\\n"
                               "<\\/p>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"visualiserPeriod\\\"><b>Frequency Range<\\/b>: <span id=\\\"visualiserPeriodLabel\\\">2000<\\/span>Hz<\\/label>\\r\\n"
                               "    <input id=\\\"visualiserPeriod\\\" type=\\\"range\\\" min=\\\"100\\\" max=\\\"2000\\\" step=\\\"50\\\" value=\\\"250\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"visualiserMinThreshold\\\"><b>Minimum Threshold<\\/b>: <span id=\\\"visualiserMinThresholdLabel\\\">100<\\/span><\\/label>\\r\\n"
                               "    <input id=\\\"visualiserMinThreshold\\\" type=\\\"range\\\" min=\\\"0\\\" max=\\\"300\\\" step=\\\"10\\\" value=\\\"100\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"visualiserMaxThreshold\\\"><b>Maximum Threshold<\\/b>: <span id=\\\"visualiserMaxThresholdLabel\\\">750<\\/span><\\/label>\\r\\n"
                               "    <input id=\\\"visualiserMaxThreshold\\\" type=\\\"range\\\" min=\\\"400\\\" max=\\\"3000\\\" step=\\\"50\\\" value=\\\"750\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"visualiserFadeUp\\\"><b>Fade Up Delay<\\/b>: <span id=\\\"visualiserFadeUpLabel\\\">32<\\/span><\\/label>\\r\\n"
                               "    <input id=\\\"visualiserFadeUp\\\" type=\\\"range\\\" min=\\\"0\\\" max=\\\"255\\\" step=\\\"1\\\" value=\\\"32\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"visualiserFadeDown\\\"><b>Fade Down Delay<\\/b>: <span id=\\\"visualiserFadeDownLabel\\\">32<\\/span><\\/label>\\r\\n"
                               "    <input id=\\\"visualiserFadeDown\\\" type=\\\"range\\\" min=\\\"0\\\" max=\\\"255\\\" step=\\\"1\\\" value=\\\"32\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n"
                               "<div>\\r\\n"
                               "    <label for=\\\"visualiserHueOffset\\\"><b>Hue Offset<\\/b>: <span id=\\\"visualiserHueOffsetLabel\\\">170<\\/span><\\/label>\\r\\n"
                               "    <input id=\\\"visualiserHueOffset\\\" type=\\\"range\\\" min=\\\"0\\\" max=\\\"359\\\" step=\\\"1\\\" value=\\\"170\\\" class=\\\"form-control-range custom-range\\\">\\r\\n"
                               "<\\/div>\\r\\n"
                               "<h6 class=\\\"pt-4\\\">Variable Description<\\/h6>\\r\\n"
                               "<ul>\\r\\n"
                               "    <li><b>Frequency Range<\\/b> - The displayed frequency spectrum from left to right starting from 0Hz<\\/li>\\r\\n"
                               "    <li><b>Minimum Threshold<\\/b> - The minimum value of the FFT calculations to map to the zero brightness\\r\\n"
                               "        of the LED's, useful to remove noise<\\/li>\\r\\n"
                               "    <li><b>Maximum Threshold<\\/b> - The maximum ceiling value value of the FFT to map to max brightness of\\r\\n"
                               "        the LED's<\\/li>\\r\\n"
                               "    <li><b>Fade Up Delay<\\/b> - A multiplier to control the speed of animation when setting the light to new\\r\\n"
                               "        brightness<\\/li>\\r\\n"
                               "    <li><b>Fade Down Delay<\\/b> - A multiplier to control how fast the LED's fade out when no new brightness\\r\\n"
                               "        has been set<\\/li>\\r\\n"
                               "    <li><b>Hue Offset<\\/b> - The offset hue value from 0 for the start of the rainbow<\\/li>\\r\\n"
                               "<\\/ul>\\r\\n");
  }

  virtual const char *getTabScript()
  {
    return PSTR("var visualiserDebunce = Date.now()\\r\\n"
                                 "var lastVisualiserMessaeg = \\\"\\\"\\r\\n"
                                 "\\r\\n"
                                 "messageEventList.push(handleVisualiserMessage)\\r\\n"
                                 "\\r\\n"
                                 "$(\\\"#visualiserPeriod\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserPeriod\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserMinThreshold\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserMinThreshold\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserMaxThreshold\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserMaxThreshold\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserFadeUp\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserFadeUp\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserFadeDown\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserFadeDown\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserHueOffset\\\").on(\\\"input\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "$(\\\"#visualiserHueOffset\\\").on(\\\"change\\\", function () {\\r\\n"
                                 "    onVisualiserEvent()\\r\\n"
                                 "});\\r\\n"
                                 "\\r\\n"
                                 "function handleVisualiserMessage(jsonMessage) {\\r\\n"
                                 "    if (\\\"Visualiser\\\" in jsonMessage) {\\r\\n"
                                 "        jsonMessage = jsonMessage.Visualiser\\r\\n"
                                 "        if (typeof jsonMessage === \\\"object\\\") {\\r\\n"
                                 "            if ((\\\"Period\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.Period === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#visualiserPeriod\\\").val(jsonMessage.Period)\\r\\n"
                                 "                    $(\\\"#visualiserPeriodLabel\\\").html(Math.round(1000000 / jsonMessage.Period / 2))\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"MinThreshold\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.MinThreshold === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#visualiserMinThreshold\\\").val(jsonMessage.MinThreshold)\\r\\n"
                                 "                    $(\\\"#visualiserMinThresholdLabel\\\").html(jsonMessage.MinThreshold)\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"MaxThreshold\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.MaxThreshold === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#visualiserMaxThreshold\\\").val(jsonMessage.MaxThreshold)\\r\\n"
                                 "                    $(\\\"#visualiserMaxThresholdLabel\\\").html(jsonMessage.MaxThreshold)\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"FadeUp\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.FadeUp === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#visualiserFadeUp\\\").val(jsonMessage.FadeUp)\\r\\n"
                                 "                    $(\\\"#visualiserFadeUpLabel\\\").html(jsonMessage.FadeUp)\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"FadeDown\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.FadeDown === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#visualiserFadeDown\\\").val(jsonMessage.FadeDown)\\r\\n"
                                 "                    $(\\\"#visualiserFadeDownLabel\\\").html(jsonMessage.FadeDown)\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "            if ((\\\"HueOffset\\\" in jsonMessage)) {\\r\\n"
                                 "                if (typeof jsonMessage.HueOffset === \\\"number\\\") {\\r\\n"
                                 "                    $(\\\"#visualiserHueOffset\\\").val(Math.round(jsonMessage.HueOffset / 255 * 360))\\r\\n"
                                 "                    $(\\\"#visualiserHueOffsetLabel\\\").html(Math.round(jsonMessage.HueOffset / 255 * 360))\\r\\n"
                                 "                }\\r\\n"
                                 "            }\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n"
                                 "\\r\\n"
                                 "function onVisualiserEvent() {\\r\\n"
                                 "    let currentPeriod = parseInt($(\\\"#visualiserPeriod\\\").val(), 10)\\r\\n"
                                 "    let currentMinThreshold = parseInt($(\\\"#visualiserMinThreshold\\\").val(), 10)\\r\\n"
                                 "    let currentMaxThreshold = parseInt($(\\\"#visualiserMaxThreshold\\\").val(), 10)\\r\\n"
                                 "    let currentFadeUp = parseInt($(\\\"#visualiserFadeUp\\\").val(), 10)\\r\\n"
                                 "    let currentFadeDown = parseInt($(\\\"#visualiserFadeDown\\\").val(), 10)\\r\\n"
                                 "    let currentHueOffset = parseInt($(\\\"#visualiserHueOffset\\\").val(), 10)\\r\\n"
                                 "\\r\\n"
                                 "    $(\\\"#visualiserPeriodLabel\\\").html(Math.round(1000000 / currentPeriod / 2))\\r\\n"
                                 "    $(\\\"#visualiserMinThresholdLabel\\\").html(currentMinThreshold)\\r\\n"
                                 "    $(\\\"#visualiserMaxThresholdLabel\\\").html(currentMaxThreshold)\\r\\n"
                                 "    $(\\\"#visualiserFadeUpLabel\\\").html(currentFadeUp)\\r\\n"
                                 "    $(\\\"#visualiserFadeDownLabel\\\").html(currentFadeDown)\\r\\n"
                                 "    $(\\\"#visualiserHueOffsetLabel\\\").html(currentHueOffset)\\r\\n"
                                 "\\r\\n"
                                 "    msg = {\\r\\n"
                                 "        \\\"State\\\": true,\\r\\n"
                                 "        \\\"Mode\\\": \\\"Visualiser\\\",\\r\\n"
                                 "        \\\"Visualiser\\\": {\\r\\n"
                                 "            \\\"Period\\\": currentPeriod,\\r\\n"
                                 "            \\\"MinThreshold\\\": currentMinThreshold,\\r\\n"
                                 "            \\\"MaxThreshold\\\": currentMaxThreshold,\\r\\n"
                                 "            \\\"FadeUp\\\": currentFadeUp,\\r\\n"
                                 "            \\\"FadeDown\\\": currentFadeDown,\\r\\n"
                                 "            \\\"HueOffset\\\": Math.round(currentHueOffset / 360 * 255)\\r\\n"
                                 "        }\\r\\n"
                                 "    }\\r\\n"
                                 "\\r\\n"
                                 "    if (msg != lastVisualiserMessaeg && Date.now() - visualiserDebunce > 50) {\\r\\n"
                                 "        lastVisualiserMessaeg = msg\\r\\n"
                                 "        visualiserDebunce = Date.now()\\r\\n"
                                 "        sendMessage(msg)\\r\\n"
                                 "    }\\r\\n"
                                 "}\\r\\n");
  }
};
