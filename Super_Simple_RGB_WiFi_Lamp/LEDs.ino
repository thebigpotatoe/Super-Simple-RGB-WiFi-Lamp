void ledStringInit() {
  // add the leds to fast led and clear them
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(ledString, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  // Set the maximum power draw
  // FastLED.setMaxPowerInVoltsAndMilliamps(5,1000); 

  // Debug
  Serial.println("[handleMode] - LED string was set up correctly");
}

void handleMode() {
  // Adapt the leds to the current mode
  if (currentMode == "Colour") {
    setColour(colourRed, colourGreen, colourBlue);
  }
  else if (currentMode == "Rainbow") {
    setRainbow(rainbowStartHue, rainbowSpeed);
  }
  else if (currentMode == "Clock") {
    setClock();
  }
  else if (currentMode == "Bell Curve") {
    setBellCurve();
  }
  else if (currentMode == "Night Rider") {
    setNightRider();
  }
  else if (currentMode == "Sparkle") {
    setSparkle(sparkleSpeed);
  }
  else if (currentMode == "Color Wipe") {
    setColorWipe();
  }
  else if (currentMode == "Confetti") {
    setConfetti(confettiSpeed);
  }
  else if ( currentMode == "Visualiser" ) {
    setVisualiser();
  }

  // Adjust the brightness depending on the mode
  if (autoOnWithModeChange || State) {
    if (Mode != currentMode) {
      // Dim lights off first 
      if (modeChangeFadeAmount > 0) {
        // Set the dimming variables and apply
        EVERY_N_MILLISECONDS(20) {
          modeChangeFadeAmount -= (FadeTime > 0) ? (255 / ((float)FadeTime/20)) : 255;
          modeChangeFadeAmount = constrain(modeChangeFadeAmount, 0, 255);
        };
      }
      else {
        // Debug
        Serial.println("[handleMode] - Mode changed to: " + Mode);

        // Clear the LEDs
        FastLED.clear();

        // Set the currentMode to Mode
        currentMode = Mode;
        modeChangeFadeAmount = 0;
      }
    }
    else if (currentMode != previousMode) {
      // On mode change dim lights up
      if (modeChangeFadeAmount < 255) {
        EVERY_N_MILLISECONDS(20) {
          modeChangeFadeAmount += (FadeTime > 0) ? (255 / ((float)FadeTime/20)) : 255;
          modeChangeFadeAmount = constrain(modeChangeFadeAmount, 0, 255);
        };
      }
      else {
        // Set the currentMode to Mode
        previousMode = currentMode;
      }
    } 
  }
  
  // Adjust the brightness depending on the state
  if (!State && previousState) {
    // Turn Lights off slowly
    if (modeChangeFadeAmount > 0) {
      EVERY_N_MILLISECONDS(20) {
        modeChangeFadeAmount -= (FadeTime > 0) ? (255 / ((float)FadeTime/20)) : 255;
        modeChangeFadeAmount = constrain(modeChangeFadeAmount, 0, 255);
      };
    }
    else {
      // Debug 
      Serial.println("[handleMode] - LED's turned off");

      // Set the previous state
      previousState = false;
    }
  }
  else if (State && !previousState) {
      // Turn on light slowly
      if (modeChangeFadeAmount < 255) {
        EVERY_N_MILLISECONDS(20) {
          modeChangeFadeAmount += (FadeTime > 0) ? (255 / ((float)FadeTime/20)) : 255;
          modeChangeFadeAmount = constrain(modeChangeFadeAmount, 0, 255);
        };
      }
      else {
        // Debug 
        Serial.println("[handleMode] - LED's turned on");

        // Set the previous values
        previousState = true;
      }
    } 

  // Globally Scale the brightness of all LED's
  nscale8(ledString, NUM_LEDS, (int)modeChangeFadeAmount);

  // Handle Fast LED
  brightness = constrain(brightness, 0, 255);
  FastLED.setBrightness(brightness);
  FastLED.show();
  //  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void setColour(int red, int green, int blue) {
  fill_solid(ledString, NUM_LEDS, CRGB(red, green, blue));
}

void setRainbow(int startHue, int speed) {
  // Constrain the variables before using
  startHue = constrain(startHue, 0, 255);
  speed = speed > 0 ? speed : 0;
  brightness = constrain(brightness, 0, 255);

  
  // Update the hue by 1 every 360th of the allocated time
  if (speed > 0) {
    float rainbowDeltaHue = (255 / ((float)speed * 1000)) * 50;
    EVERY_N_MILLISECONDS(50) {
      rainbowAddedHue += rainbowDeltaHue;
      rainbowAddedHue = (rainbowAddedHue > 255) ? rainbowAddedHue - 255 : rainbowAddedHue;
      ledString[ random16(NUM_LEDS) ] += CRGB::White;
    };

    startHue += (int)rainbowAddedHue;
  }

  // Calculate the rainbow so it lines up
  float deltaHue = (float)255/(float)NUM_LEDS;
  float currentHue = startHue;
  for (int i = 0; i < NUM_LEDS; i++) {
    currentHue = startHue + (float)(deltaHue*i);
    currentHue = (currentHue < 255) ? currentHue : currentHue - 255;
    ledString[i] = CHSV( currentHue, 255, 255);
  }
}


void setClock() {
  if (ntpTimeSet) {
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

    // Clear all the LEDs
    FastLED.clear();

    // Set the colour of the LED
    ledString[hourCurrentLED] = CRGB( clockHourRed, clockHourGreen, clockHourBlue);
    ledString[hourNextLED] = ledString[hourCurrentLED];
    ledString[minuteCurrentLED] = CRGB( clockMinRed, clockMinGreen, clockMinBlue);
    ledString[minuteNextLED] = ledString[minuteCurrentLED];

    // Dim the led correctly
    ledString[hourCurrentLED].nscale8(hourCurrentLEDBrightness);
    ledString[hourNextLED].nscale8(hourNextLEDBrightness);
    ledString[minuteCurrentLED].nscale8(minuteCurrentLEDBrightness);
    ledString[minuteNextLED].nscale8(minuteNextLEDBrightness); 
  }
  else {
    // Set each of the lights colours
    for (int i = 0; i < topNumLeds; i++){
      ledString[topLeds[i]] = CRGB(clockHourRed, clockHourGreen, clockHourBlue);
    }
    for (int i = 0; i < topNumLeds; i++){
      ledString[bottomLeds[i]] = CRGB(clockMinRed, clockMinGreen, clockMinBlue);
    }
    
    // Set the brightness up and down
    // Serial.println(sin8(clockOnPauseBrightness));
    nscale8(ledString, NUM_LEDS, triwave8(clockOnPauseBrightness));
    clockOnPauseBrightness += 1;
  }
}

void setBellCurve() {
  // Set the top brightness
  for (int i = 0; i < topNumLeds; i++) {
    int ledNrightness = cubicwave8( ( 255 / (float)topNumLeds  ) * i );
    ledString[topLeds[i]] = CRGB(bellCurveRed, bellCurveGreen, bellCurveBlue);
    ledString[topLeds[i]] %= ledNrightness;
  }

  // Set the Bottom brightness
  for (int i = 0; i < bottomNumLeds; i++) {
    int ledNrightness = cubicwave8( ( 255 / (float)bottomNumLeds  ) * i );
    ledString[bottomLeds[i]] = CRGB(bellCurveRed, bellCurveGreen, bellCurveBlue);
    ledString[bottomLeds[i]] %= ledNrightness;
  }
}

void setNightRider() {
  int delayTime = 500 / topNumLeds;
  EVERY_N_MILLISECONDS(delayTime) {
    // Set the current LED to Red
    ledString[topLeds[nightRiderTopLedNumber]] = CRGB(255, 0, 0);
    ledString[bottomLeds[nightRiderBottomLedNumber]] = CRGB::Red;
    // Serial.println(nightRiderTopLedNumber);
    // Serial.println(ledString[topLeds[0]]);

    // Increment the LED number
    nightRiderTopLedNumber = nightRiderTopLedNumber + nightRiderTopIncrement;
    nightRiderBottomLedNumber = nightRiderBottomLedNumber + nightRiderBottomIncrement;
    if (nightRiderTopLedNumber >= topNumLeds - 1 || nightRiderTopLedNumber <= 0) nightRiderTopIncrement = -nightRiderTopIncrement;
    if (nightRiderBottomLedNumber >= bottomNumLeds - 1 || nightRiderBottomLedNumber <= 0) nightRiderBottomIncrement = -nightRiderBottomIncrement;

    // Start fading all lit leds
    fadeToBlackBy( ledString, NUM_LEDS, 10);
  };
}

void setSparkle(int speed) {
  EVERY_N_MILLISECONDS(sparkleSpeed) {
    if (sparkleActive) {
      sparklePixel = random(NUM_LEDS);
      ledString[sparklePixel] = CRGB(sparkleRed, sparkleGreen, sparkleBlue);
    } 
    else {
      ledString[sparklePixel] = CRGB(0, 0, 0);
      sparkleActive = !sparkleActive;
    }
  }
}

void setColorWipe() {
  EVERY_N_MILLISECONDS(colorWipeSpeed) {
    colorWipePosition++;
    if (TurningOn) {
      fill_solid(ledString, colorWipePosition, CRGB(colorWipeRed, colorWipeGreen, colorWipeBlue));
      if (colorWipePosition == NUM_LEDS) {
        TurningOn = false;
        colorWipePosition = -1;
      }
    }
    else {
      fill_solid(ledString, colorWipePosition, CRGB( 0, 0, 0));
      if (colorWipePosition == NUM_LEDS) {
        TurningOn = true;
        colorWipePosition = -1;
      }
    }
  }
}

void setConfetti(int speed) {
  EVERY_N_MILLISECONDS(confettiSpeed) {
    if (confettiActive) {
      confettiPixel = random(NUM_LEDS);
      fadeToBlackBy(ledString, NUM_LEDS, 10);
      uint8_t pos = random8(NUM_LEDS);
      ledString[pos] += CHSV(random8(), random8(), random8());
    }
  }
}

void setVisualiser() {
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
    while (sampleNumber < num_samples) {
      // Get the ADC reading 
      adcBufferTime = micros();
      system_adc_read_fast(sampleBuffer, 1, 8);
      
      // If the correct period of time has passed store the reading
      if (sampleNumber < num_samples && adcBufferTime - visualiserLastSampleTime > visualiserPeriod) {
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
    // for (int i = 0; i < num_samples; i++) {
    //   Serial.println(visualiserRealSamples[i]);
    // }
  
    // ************* Calculate FFT *************
    FFT.Windowing(visualiserRealSamples, num_samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); 
    FFT.Compute(visualiserRealSamples, visualiserImaginarySamples, num_samples, FFT_FORWARD); 
    FFT.ComplexToMagnitude(visualiserRealSamples, visualiserImaginarySamples, num_samples); 
  
    // ************* Set the LEDs *************
    // Set the colour of each light based on the values calculated
    for (int ledNum = 0; ledNum < topNumLeds; ledNum++) {
      // Map to the bin number, skip all bins required. Start at the second bin to avoid DC
      uint8_t binNumber = (ledNum < visualiserNumBinsToSkip) ? visualiserNumBinsToSkip : constrain(ledNum, 0, num_samples/2);
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
