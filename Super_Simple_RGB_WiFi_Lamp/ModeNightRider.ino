class ModeNightRider : public ModeBase
{
private:
    int nightRiderTopLedNumber;
    int nightRiderBottomLedNumber;
    int nightRiderTopIncrement;
    int nightRiderBottomIncrement;
public:
    ModeNightRider() {}

    virtual void initialize() {
        nightRiderTopLedNumber    = 0;
        nightRiderBottomLedNumber = 0;
        nightRiderTopIncrement    = 1;
        nightRiderBottomIncrement = 1;
    }

    virtual void render() {
        int delayTime = 500 / topNumLeds;
        EVERY_N_MILLISECONDS(delayTime) {
          // Set the current LED to Red
          ledString[topLeds[nightRiderTopLedNumber]] = CRGB(255, 0, 0);
          ledString[bottomLeds[nightRiderBottomLedNumber]] = CRGB::Red;
          // Serial.println(nightRiderTopLedNumber);
          // Serial.println(ledString[topLeds[0]]);

          //  Increment the LED number
          nightRiderTopLedNumber = nightRiderTopLedNumber + nightRiderTopIncrement;
          nightRiderBottomLedNumber = nightRiderBottomLedNumber + nightRiderBottomIncrement;
          if (nightRiderTopLedNumber >= topNumLeds - 1 || nightRiderTopLedNumber <= 0) nightRiderTopIncrement = -nightRiderTopIncrement;
          if (nightRiderBottomLedNumber >= bottomNumLeds - 1 || nightRiderBottomLedNumber <= 0) nightRiderBottomIncrement = -nightRiderBottomIncrement;

          // Start fading all lit leds
          fadeToBlackBy( ledString, NUM_LEDS, 10);
        };
    }

    virtual void applyConfig(JsonVariant& settings) {

    }
};
