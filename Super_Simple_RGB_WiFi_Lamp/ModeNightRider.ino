class ModeNightRider : public ModeBase
{
private:
    int nightRiderTopLedNumber = 0;
    int nightRiderBottomLedNumber = 0;
    int nightRiderTopIncrement = 1;
    int nightRiderBottomIncrement = 1;

public:
    ModeNightRider() {}
    virtual void render()
    {
        int delayTime = 500 / topNumLeds;
        EVERY_N_MILLISECONDS(delayTime)
        {
            // Set the current LED to Red
            ledString[topLeds[nightRiderTopLedNumber]] = CRGB(255, 0, 0);
            ledString[bottomLeds[nightRiderBottomLedNumber]] = CRGB::Red;
            // Serial.println(nightRiderTopLedNumber);
            // Serial.println(ledString[topLeds[0]]);

            //  Increment the LED number
            nightRiderTopLedNumber = nightRiderTopLedNumber + nightRiderTopIncrement;
            nightRiderBottomLedNumber = nightRiderBottomLedNumber + nightRiderBottomIncrement;
            if (nightRiderTopLedNumber >= topNumLeds - 1 || nightRiderTopLedNumber <= 0)
                nightRiderTopIncrement = -nightRiderTopIncrement;
            if (nightRiderBottomLedNumber >= bottomNumLeds - 1 || nightRiderBottomLedNumber <= 0)
                nightRiderBottomIncrement = -nightRiderBottomIncrement;

            // Start fading all lit leds
            fadeToBlackBy(ledString, NUM_LEDS, 10);
        };
    }

    virtual void applyConfig(JsonVariant &settings)
    {
    }

    virtual const char *getName()
    {
        return "Night Rider";
    }

    virtual const char *getTabHtml()
    {
        return PSTR("<h2>Night Rider Mode<\\/h2>\\r\\n"
                                   "<p>Knight Rider. A shadowy flight into the dangerous world of a man who does not exist.\\r\\n"
                                   "    Michael Knight: a young loner on a crusade to champion the cause of the innocent,\\r\\n"
                                   "    the helpless, the powerless, in a world of criminals who operate above the law.<\\/p>\\r\\n");
    }

    virtual const char *getTabScript()
    {
        return PSTR("messageEventList.push(handleNightRiderMessage)\\r\\n"
                                     "\\r\\n"
                                     "function handleNightRiderMessage(jsonMessage) {\\r\\n"
                                     "    if (\\\"Night Rider\\\" in jsonMessage) {\\r\\n"
                                     "        jsonMessage = jsonMessage[\\\"Night Rider\\\"]\\r\\n"
                                     "        if (typeof jsonMessage === \\\"object\\\") {\\r\\n"
                                     "\\r\\n"
                                     "        }\\r\\n"
                                     "    }\\r\\n"
                                     "}\\r\\n");
    }
};
