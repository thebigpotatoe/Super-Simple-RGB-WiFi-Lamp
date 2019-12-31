class ModeCircle : public ModeBase
{
private:
    int circleActiveLedNumber;
public:
    ModeCircle() {}

    virtual void initialize() {
        circleActiveLedNumber = 0;
    }

    virtual void render() {
        // First bring our logical arrays into a list of led numbers to iterate over
        int i;
        int ledIter = 0;
        int leds[NUM_LEDS];
        for (i = 0; i < bottomNumLeds; i++) {
          leds[ledIter++] = bottomLeds[i];
        }
        for (i = leftNumLeds-1; i >= 0 ; i--) {
          leds[ledIter++] = leftLeds[i];
        }
        for (i = topNumLeds-1; i >= 0 ; i--) {
          leds[ledIter++] = topLeds[i];
        }
        for (i = rightNumLeds-1; i >= 0 ; i--) {
          leds[ledIter++] = rightLeds[i];
        }

        // Update the active LED index
        EVERY_N_MILLISECONDS(40) {
          circleActiveLedNumber += 1;
          if (circleActiveLedNumber == NUM_LEDS)
              circleActiveLedNumber = 0;

          // Darken all LEDs to slightly dim the previous active LEDs
          fadeToBlackBy(ledString, NUM_LEDS, 80);
        };

        // And now highlight the active index
        for (i = 0; i < NUM_LEDS; i++) {
          if (i == circleActiveLedNumber) {
            ledString[leds[i]] = CRGB::Red;
          }
        }
    }

    virtual void applyConfig(JsonVariant& settings) {

    }
};
