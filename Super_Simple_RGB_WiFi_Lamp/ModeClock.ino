class ModeClock : public ModeBase
{
public:
    ModeClock() {}
    virtual void render() {
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

            // Clear all the LED's
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
};
