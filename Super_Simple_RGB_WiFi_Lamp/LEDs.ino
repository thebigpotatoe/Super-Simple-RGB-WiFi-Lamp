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
  // Adapt the leds to the current mode. Please note the differences between Mode and currentMode.
  //
  // Mode:        Is set by the config or the web interface to tell the lamp that a specific mode should
  //              be shown. The lamp will then change to that mode slowly (see adjustBrightnessAndSwitchMode())
  // currentMode: In case this is set to a mode name, the lamp came to the point where a mode should be
  //              asked to render it's pattern.
  //              During startup this is set to "". In this situation the mode should not be rendered, but
  //              adjustBrightnessAndSwitchMode() should be called to introduce Mode.

  if (currentMode != "") {
    auto modeIter = modes.find(currentMode);
    if (modeIter == modes.end()) {
      // Should only be reached when a user has configured a mode that does not exist (anymore)
      Serial.println("[handleMode] - Mode \"" + currentMode + "\" not found, resetting to default");
      Mode = "Colour"; // Automatically jump back to colour
      return;
    }

    // If mode is found run its render function
    modeIter->second->render();
  }

  // Globally adjust the brightness
  adjustBrightnessAndSwitchMode();

  // Handle Fast LED
  FastLED.show();
  //  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void adjustBrightnessAndSwitchMode() {
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
}
