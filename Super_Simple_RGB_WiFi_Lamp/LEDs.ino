void ledStringInit() {
  // add the leds to fast led and clear them
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(ledString, NUM_LEDS);
  FastLED.clear ();
  FastLED.show();

  // Set the maximum power draw
  // FastLED.setMaxPowerInVoltsAndMilliamps(5,1000);

  // Debug
  Serial.println("[handleMode] - LED string was set up correctly");
}

void handleMode() {
  // Adapt the leds to the current mode

  auto modeIter = modes.find(currentMode);
  if (modeIter == modes.end()) {
    // not found
    Serial.println("[handleMode] - Mode \"" + Mode + "\" not found, resetting to default");
    Mode = "Colour";    // Automatically jump back to colour
    currentMode = "Colour";    // Automatically jump back to colour
    return;
  }
  else {
    // Apply currentMode always?

    // If mode is found run its render function
    modeIter->second->render();

    // Globally adjust the brightness
    adjustBrightness();

    // Handle Fast LED
    FastLED.show();
    //  FastLED.delay(1000 / FRAMES_PER_SECOND);
  }  
}

void adjustBrightness() {
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

        // Initialize state of the new mode
        auto modeIter = modes.find(Mode);
        if (modeIter != modes.end()) {
          modeIter->second->initialize();
        }

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
}
