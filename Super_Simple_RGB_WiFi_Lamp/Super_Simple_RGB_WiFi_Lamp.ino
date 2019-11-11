// Included Libraries
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "user_interface.h"
#include <FastLED.h>
#include "FS.h"
#include <ESP8266WiFi.h>
#include "IPAddress.h"
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESPAsyncTCP.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <ESPAsyncUDP.h>
#include "arduinoFFT.h"
#include "lwip/inet.h"
#include "lwip/dns.h"

// ############################################################# Sketch Variables #############################################################
// All variables at the top of this sketch need to be defined correctly for your light. Read the comments around each one for more details on 
// what each of them are.
#define DEFAULT_NAME "Super Simple RGB WiFi Lamp"

// Set Your Data pin - This is the pin on your ESP8266 that is connected to the LED's. Be careful as on the NodeMCU the D pin does not map to 
// pin number. For this example pin D1 on the NodeMCU is actually pin 5 in software.
#define DATA_PIN 5

// Set the number of LED's - Simply count how many there are on your string and enter the number here
#define NUM_LEDS 152

// Set your UTC offset - This is the time zone you are in. for example +10 for Sydney or -4 for NYC
#define UTC_OFFSET -5

// Set the chipset and color order for the LEDs you are using. For more info on supported hardware see: https://github.com/FastLED/FastLED/wiki/Overview#supported-chipsets.
#define CHIPSET WS2812B
#define COLOR_ORDER GRB

// Set up LED's for each side - These arrays hold which leds are on what sides. For the basic rectangular shape in the example this relates to 4
// sides and 4 arrays. You must subract 1 off the count of the LED when entering it as the array is 0 based. For example the first LED on the 
// string is entered as 0.
int topLeds[]     = {43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109};
int bottomLeds[]  = {33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 151, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119};
int leftLeds[]    = {110, 111, 112, 113, 114, 115, 116, 117, 118};
int rightLeds[]   = {34, 35, 36, 37, 38, 39, 40, 41, 42};

// Eneter your wifi credentials here - If you would like to enter your wifi credentials now you can with these variables. This is a nice easy 
// method to get your ESP8266 connected to your network quickly. If you dont you can always set it up later in the wifi portal.
String SSID = "MineIsBetterThanYours";
String Password = "Thatswhatshesaid";
// ########################################################## End of Sketch Variables ##########################################################

// File System Variables 
bool spiffsCorrectSize      = false;

// Wifi Variables and Objects 
String programmedSSID       = SSID;
String programmedPassword   = Password;
bool wifiStarting           = false;
bool softApStarted          = false;
IPAddress accessPointIP     = IPAddress(192, 168, 1, 1);
WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

// DNS and mDNS Objects
DNSServer captivePortalDNS;
MDNSResponder::hMDNSService mdnsService;

// Webserver and OTA Objects
ESP8266WebServer restServer(80);
ESP8266HTTPUpdateServer OTAServer;

// Web Sockets Variabels and Objects
WebSocketsServer webSocket(81);
bool processingMessage = false;
bool clientNeedsUpdate = false;
bool webSocketConnecting = false;

// NTP Variables and Objects
AsyncUDP udpClient;
bool ntpTimeSet                       = false;
String ntpHostName                    = "pool.ntp.org";
IPAddress ntpIpAddress                = IPAddress(0, 0, 0, 0);
unsigned long utcOffset               = UTC_OFFSET * 3600; // in seconds
unsigned long collectionPeriod        = 3600;
unsigned long currentEpochTime        = 0;
unsigned long lastNTPCollectionTime   = 0;

// LED string object and Variables
CRGB ledString[NUM_LEDS];
bool autoOnWithModeChange = true;
int topNumLeds      = sizeof(topLeds) / sizeof(*topLeds);
int bottomNumLeds   = sizeof(bottomLeds) / sizeof(*bottomLeds);
int leftNumLeds     = sizeof(leftLeds) / sizeof(*leftLeds);
int rightNumLeds    = sizeof(rightLeds) / sizeof(*rightLeds);

// Base Variables of the Light
String  Name                  = DEFAULT_NAME;                         // The default Name of the Device
String  Mode                  = "";                                   // The default Mode of the Device
bool    State                 = true;                                 // The Default Mode of the Light
int     FadeTime              = 200;                                  // Fading time between states in ms
int     brightness            = 100;                                  // LED brightness
String  currentMode           = Mode;                                 // Placeholder variable for changing mode
String  previousMode          = "";                                   // Placeholder variable for changing mode
bool    previousState         = false;                                // Placeholder variable for changing state
float   modeChangeFadeAmount  = 0;                                    // Place holder for global brightness during mode change

// Colour Mode Variables 
int colourRed                     = 128;
int colourGreen                   = 128;
int colourBlue                    = 128;

// Rainbow Mode Variables
int rainbowStartHue               = 0;
int rainbowSpeed                  = 10;
int rainbowBri                    = 100;
float rainbowAddedHue             = 0;

// Clock Mode Variables 
int clockHourRed                  = 128;
int clockHourGreen                = 128;
int clockHourBlue                 = 128;
int clockMinRed                   = 128;
int clockMinGreen                 = 128;
int clockMinBlue                  = 128;
int clockOnPauseBrightness        = 255;
unsigned long lastClockExecution  = 0;

// Bell Curve Mode Variables
int bellCurveRed                  = 128;
int bellCurveGreen                = 128;
int bellCurveBlue                 = 128;

// Night Rider Mode Variables
int nightRiderTopLedNumber        = 0;
int nightRiderBottomLedNumber     = 0;
int nightRiderTopIncrement        = 1;
int nightRiderBottomIncrement     = 1;

// Sparkle Mode Variables
int sparkleSpeed                  = 30;
bool sparkleActive                = true;
int sparkleRed                    = 128;
int sparkleGreen                  = 128;
int sparkleBlue                   = 128;
int sparklePixel                  = random(NUM_LEDS);

// Color wipe Variables
int colorWipePosition             = -1;
bool TurningOn                    = true;
int colorWipeRed                  = 255;
int colorWipeGreen                = 0;
int colorWipeBlue                 = 255;
int colorWipeSpeed                = 20;

// Confetti Variables
bool confettiActive               = true;
int confettiRed                   = 128;
int confettiGreen                 = 128;
int confettiBlue                  = 128;
int confettiSpeed                 = 100;
int confettiPixel                 = random(NUM_LEDS);
int hue                           = 0;

// Visualiser Mode Variables
#define num_samples 64
ADC_MODE(ADC_TOUT);
arduinoFFT FFT = arduinoFFT();
double visualiserRealSamples[num_samples];
double visualiserImaginarySamples[num_samples];
unsigned long visualiserLastSampleTime      = 0;
uint16_t visualiserPeriod                   = 250;
uint16_t visualiserMinThreshold             = 100;
uint16_t visualiserMaxThreshold             = 750;
uint8_t visualiserNumBinsToSkip             = 3;
uint8_t visualiserFadeUp                    = 32;
uint8_t visualiserFadeDown                  = 32;
uint8_t visualiserHueOffset                 = 170;

// Setup Method - Runs only once before the main loop. Useful for setting things up
void setup() {
  // Add a short delay on start
  delay(1000);

  // Start Serial
  Serial.begin(115200);
  Serial.println();

  // Check if the flash has been set up correctly
  spiffsCorrectSize = checkFlashConfig();
  if (spiffsCorrectSize) {
    // Init the LED's
    ledStringInit();

    // Get saved settings
    getConfig();

    // Start Wifi
    wifiInit();

    // Setup Webserver
    webServerInit();

    // Setup websockets
    websocketsInit();
  }
  else Serial.println("[setup] -  Flash configuration was not set correctly. Please check your settings under \"tools->flash size:\"");
}

// The Main Loop Methdo - This runs continuously
void loop() {
  // Check if the flash was correctly setup
  if (spiffsCorrectSize) {
    // Handle the captive portal 
    captivePortalDNS.processNextRequest();

    // Handle mDNS 
    MDNS.update();

    // // Handle the webserver
    restServer.handleClient();
    
    // Handle Websockets
    webSocket.loop();

    // Get the time when needed
    handleNTP();

    // Update WS clients when needed
    updateClients();

    // Handle the wifi connection 
    handleWifiConnection();

    // Update the LED's
    handleMode();
  }
  else {
    delay(10000);
    Serial.println("[loop] - Flash configuration was not set correctly. Please check your settings under \"tools->flash size:\"");
  }
}
