class ModeColour : public ModeBase
{
public:
    ModeColour() {}
    virtual void render() {
        fill_solid(ledString, NUM_LEDS, CRGB(colourRed, colourGreen, colourBlue));
    }
};
