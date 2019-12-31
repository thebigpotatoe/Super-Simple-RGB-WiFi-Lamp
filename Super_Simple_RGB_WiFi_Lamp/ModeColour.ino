class ModeColour : public ModeBase
{
private:
    // Config
    int colourRed   = 128;
    int colourGreen = 128;
    int colourBlue  = 128;
public:
    ModeColour() {}
    virtual void initialize() {}

    virtual void render() {
        fill_solid(ledString, NUM_LEDS, CRGB(colourRed, colourGreen, colourBlue));
    }

    virtual void applyConfig(JsonVariant& settings) {
        settings["Red"] = colourRed = settings["Red"] | colourRed;
        settings["Green"] = colourGreen = settings["Green"] | colourGreen;
        settings["Blue"] = colourBlue = settings["Blue"] | colourBlue;
    }
};
