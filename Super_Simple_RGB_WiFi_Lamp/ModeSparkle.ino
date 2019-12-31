class ModeSparkle : public ModeBase
{

private:
    // Config
    int sparkleSpeed  = 30;
    int sparkleRed    = 128;
    int sparkleGreen  = 128;
    int sparkleBlue   = 128;

    // State
    bool sparkleActive;
    int sparklePixel;

public:
    ModeSparkle() {}

    virtual void initialize() {
        sparkleActive = true;
        sparklePixel  = random(NUM_LEDS);
    }

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
