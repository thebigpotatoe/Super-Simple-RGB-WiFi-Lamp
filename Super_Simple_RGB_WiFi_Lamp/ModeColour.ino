class ModeColour : public ModeBase
{
public:
    ModeColour() {}
    virtual void render() {
        fill_solid(ledString, NUM_LEDS, CRGB(colourRed, colourGreen, colourBlue));
    }

    virtual void applyConfig(JsonVariant& settings) {
        settings["Red"] = colourRed = settings["Red"] | colourRed;
        settings["Green"] = colourGreen = settings["Green"] | colourGreen;
        settings["Blue"] = colourBlue = settings["Blue"] | colourBlue;
    }
};
