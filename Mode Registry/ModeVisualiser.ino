#define VISUALISER_NUM_SAMPLES 64

class ModeVisualiser : public ModeBase
{
private:
    ADC_MODE(ADC_TOUT);
    arduinoFFT FFT = arduinoFFT();
    double visualiserRealSamples[VISUALISER_NUM_SAMPLES];
    double visualiserImaginarySamples[VISUALISER_NUM_SAMPLES];
    unsigned long visualiserLastSampleTime      = 0;
    uint16_t visualiserPeriod                   = 250;
    uint16_t visualiserMinThreshold             = 100;
    uint16_t visualiserMaxThreshold             = 750;
    uint8_t visualiserNumBinsToSkip             = 3;
    uint8_t visualiserFadeUp                    = 32;
    uint8_t visualiserFadeDown                  = 32;
    uint8_t visualiserHueOffset                 = 170;
public:
    ModeVisualiser() {}
    virtual void render() {
        // Only use visualiser when not trying to access the NTP server
        if (((WiFi.isConnected() && ntpTimeSet) || softApStarted) && !webSocketConnecting) {
          // ************* ADC Reading *************
          // Turn off interupts
          system_soft_wdt_stop();
          ets_intr_lock( ); //close interrupt
          noInterrupts();

          // Set up the buffer
          int       sampleNumber = 0;
          uint16_t  sampleBuffer[1];

          // Read the first value - seems to be invalid on first read?
          system_adc_read_fast(sampleBuffer, 1, 8);

          // Constrain the period
          visualiserPeriod = constrain(visualiserPeriod, 0, 2000);

          // Sample the ADC until buffer is full
          unsigned long adcBufferTime = micros();
          while (sampleNumber < VISUALISER_NUM_SAMPLES) {
            // Get the ADC reading
            adcBufferTime = micros();
            system_adc_read_fast(sampleBuffer, 1, 8);

            // If the correct period of time has passed store the reading
            if (sampleNumber < VISUALISER_NUM_SAMPLES && adcBufferTime - visualiserLastSampleTime > visualiserPeriod) {
              visualiserRealSamples[sampleNumber]       = sampleBuffer[0];
              visualiserImaginarySamples[sampleNumber]  = 0.0;
              visualiserLastSampleTime                  = adcBufferTime;
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
          for (int ledNum = 0; ledNum < topNumLeds; ledNum++) {
            // Map to the bin number, skip all bins required. Start at the second bin to avoid DC
            uint8_t binNumber = (ledNum < visualiserNumBinsToSkip) ? visualiserNumBinsToSkip : constrain(ledNum, 0, VISUALISER_NUM_SAMPLES/2);
            // Serial.print(binNumber);
            // Serial.print("\t");
          
            // Subract the minium value chosen for reduction of artifacts
            double adjustedBinValue = (visualiserRealSamples[binNumber] > visualiserMinThreshold) ? visualiserRealSamples[binNumber]-visualiserMinThreshold : 0.0;
            // Serial.print(adjustedBinValue);
            // Serial.print("\t");

            // Set if visualiserRealSamples[binNumber] is above 0
            if (adjustedBinValue > 0) {
              // Map the float values to 8 bit integers
              uint8_t brightnessValue =  map(adjustedBinValue, 0, visualiserMaxThreshold, 0, 255);
              // Serial.println(brightnessValue);

              // Get the current hue of the rainbow for the specific LED
              uint8_t ledHue = int(255.0/(topNumLeds - 1) * ledNum + visualiserHueOffset) % 255;
              CRGB newColour = CRGB(CHSV(ledHue, 255, 255)).nscale8(brightnessValue*(visualiserFadeUp/255.00));
          
              // Add the new colour to the current LED
              ledString[topLeds[ledNum]] = ledString[bottomLeds[ledNum]] += newColour;
          
              // If the LED num is the first or last, use it to set the sides
              if (ledNum == 0) {
                for (int sideLedNum = 0; sideLedNum < rightNumLeds; sideLedNum++){
                  ledString[rightLeds[sideLedNum]] += newColour;
                }
              }
              else if (ledNum == topNumLeds-1) {
                for (int sideLedNum = 0; sideLedNum < leftNumLeds; sideLedNum++){
                  ledString[leftLeds[sideLedNum]] += newColour;
                }
              }
            }
          }

          // Fade all leds gradually for a smooth effect
          fadeToBlackBy(ledString, NUM_LEDS, visualiserFadeDown);
          // fadeLightBy(ledString, NUM_LEDS, visualiserFadeDown);
        }
        else {
          // Serial.println("Websockets Connecting");
          delay(1000);
        }
    }

    virtual void applyConfig(JsonVariant& settings) {
        settings["Period"] = visualiserPeriod = settings["Period"] | visualiserPeriod;
        settings["MinThreshold"] = visualiserMinThreshold = settings["MinThreshold"] | visualiserMinThreshold;
        settings["MaxThreshold"] = visualiserMaxThreshold = settings["MaxThreshold"] | visualiserMaxThreshold;
        settings["FadeUp"] = visualiserFadeUp = settings["FadeUp"] | visualiserFadeUp;
        settings["FadeDown"] = visualiserFadeDown = settings["FadeDown"] | visualiserFadeDown;
        settings["HueOffset"] = visualiserHueOffset = settings["HueOffset"] | visualiserHueOffset;
    }
};
