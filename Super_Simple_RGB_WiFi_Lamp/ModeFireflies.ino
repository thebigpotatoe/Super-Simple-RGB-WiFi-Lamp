#include <FastLED.h>

class ModeFireflies : public ModeBase
{
private:
    unsigned long nextFlash[NUM_LEDS];

    unsigned int minimumFlashDelay = 1000; // in milliseconds
    unsigned int maximumFlashDelay = 5000; // in milliseconds
    int flashLength = 2500;                // in milliseconds
    int halfFlashLength = flashLength / 2;
    uint8_t brightness = 255;
    uint8_t hue = 160;

public:
    ModeFireflies()
    {
        unsigned long initialBaseTime = millis() - flashLength;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            nextFlash[i] = initialBaseTime + random16(maximumFlashDelay + flashLength);
        }
    }

    virtual void initialize() {}

    void render()
    {
        EVERY_N_MILLISECONDS(30)
        {
            unsigned long now = millis();
            long flashTime;
            int value;
            for (int i = 0; i < NUM_LEDS; i++)
            {
                flashTime = now - nextFlash[i];
                if (flashTime < 0)
                {
                    //continue;
                    value = 0;
                }
                else if (flashTime > flashLength)
                {
                    nextFlash[i] = now + random16(minimumFlashDelay, maximumFlashDelay);
                    value = 0;
                }
                else if (true)
                {
                    value = 255;
                }
                else if (flashTime > halfFlashLength)
                {
                    value = ease8InOutApprox(255 * (flashLength - flashTime) / halfFlashLength);
                }
                else
                {
                    // Brightness peaks in the very middle
                    //int value = cos(2 * PI * flashTime / flashLength) * -127 + 127;

                    // Bright peaks at 33% of the duration then falls off slowly
                    //int value = cos(2 * PI * pow(flashLength - flashTime, 2) / pow(flashLength, 2)) * -127 + 127;

                    // Ease Curve which peaks at the end
                    value = ease8InOutApprox(255 * flashTime / halfFlashLength);
                }
                ledString[i] = CHSV(hue, 255, value);
            }
            FastLED.setBrightness(brightness);
        }
    }

    virtual void applyConfig(JsonVariant &settings)
    {
        minimumFlashDelay = settings["MinimumFlashDelay"] | minimumFlashDelay;
        maximumFlashDelay = settings["MaximumFlashDelay"] | maximumFlashDelay;
        if (maximumFlashDelay < minimumFlashDelay)
        {
            std::swap(minimumFlashDelay, maximumFlashDelay);
        }
        settings["MinimumFlashDelay"] = minimumFlashDelay;
        settings["MaximumFlashDelay"] = maximumFlashDelay;

        settings["Hue"] = hue = settings["Hue"] | hue;
        settings["Brightness"] = brightness = settings["Brightness"] | brightness;
        settings["FlashLength"] = flashLength = settings["FlashLength"] | flashLength;
        halfFlashLength = flashLength / 2;
    }
};