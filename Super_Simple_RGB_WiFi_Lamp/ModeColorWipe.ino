class ModeColorWipe : public ModeBase
{
public:
    ModeColorWipe() {}
    virtual void render() {
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

    virtual void applyConfig(JsonVariant& settings) {
        settings["Red"] = colorWipeRed = settings["Red"] | colorWipeRed;
        settings["Green"]= colorWipeGreen = settings["Green"] | colorWipeGreen;
        settings["Blue"] = colorWipeBlue = settings["Blue"] | colorWipeBlue;
        settings["Speed"] = colorWipeSpeed = settings["Speed"] | colorWipeSpeed;
    }
};
