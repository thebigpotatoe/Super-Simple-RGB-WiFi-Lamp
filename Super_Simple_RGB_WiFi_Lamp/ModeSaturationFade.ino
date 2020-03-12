class ModeSaturationFade : public ModeBase
{
private:
    uint8_t fadeHue = 171;
    int fadeSpeed = 10;

public:
    ModeSaturationFade() {}

    virtual void initialize() {}

    void render()
    {
        EVERY_N_MILLISECONDS(10)
        {
            fadeSpeed = (fadeSpeed == 0) ? 1 : fadeSpeed;
            int fadeOffset = millis() / (fadeSpeed * 1000 / NUM_LEDS) % NUM_LEDS;
            for (int led = 0; led < NUM_LEDS; led++)
            {
                uint8_t saturation = sin8(((led + fadeOffset) % 255) * 255 / NUM_LEDS);
                ledString[led].setHSV(fadeHue, saturation, 100);
            }
        }
    }

    virtual void applyConfig(JsonVariant &settings)
    {
        settings["Hue"] = fadeHue = settings["Hue"] | fadeHue;
        settings["Speed"] = fadeSpeed = settings["Speed"] | fadeSpeed;
    }
};
