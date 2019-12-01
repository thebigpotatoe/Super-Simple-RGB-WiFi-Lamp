class ModeBellCurve : public ModeBase
{
public:
    ModeBellCurve() {}
    virtual void render() {
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

    virtual void applyConfig(JsonVariant& settings) {
        settings["Red"] = bellCurveRed = settings["Red"] | bellCurveRed;
        settings["Green"]= bellCurveGreen = settings["Green"] | bellCurveGreen;
        settings["Blue"] = bellCurveBlue = settings["Blue"] | bellCurveBlue;
    }
};
