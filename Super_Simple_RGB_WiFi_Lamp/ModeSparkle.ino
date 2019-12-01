class ModeSparkle : public ModeBase
{
public:
    ModeSparkle() {}
    virtual void render() {
        EVERY_N_MILLISECONDS(sparkleSpeed) {
            if (sparkleActive) {
              sparklePixel = random(NUM_LEDS);
              ledString[sparklePixel] = CRGB(sparkleRed, sparkleGreen, sparkleBlue);
            }
            else {
              ledString[sparklePixel] = CRGB(0, 0, 0);
            }
            sparkleActive = !sparkleActive;
        }
    }
};
