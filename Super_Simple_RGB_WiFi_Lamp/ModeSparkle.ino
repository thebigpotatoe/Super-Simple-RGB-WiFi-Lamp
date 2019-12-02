class ModeSparkle : public ModeBase
{

private:
    int sparkleSpeed   = 30;
    bool sparkleActive = true;
    int sparkleRed     = 128;
    int sparkleGreen   = 128;
    int sparkleBlue    = 128;
    int sparklePixel   = random(NUM_LEDS);
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

    virtual void applyConfig(JsonVariant& settings) {
        settings["Red"] = sparkleRed = settings["Red"] | sparkleRed;
        settings["Green"]= sparkleGreen = settings["Green"] | sparkleGreen;
        settings["Blue"] = sparkleBlue = settings["Blue"] | sparkleBlue;
        settings["Speed"] = sparkleSpeed = settings["Speed"] | sparkleSpeed;
    }
};
