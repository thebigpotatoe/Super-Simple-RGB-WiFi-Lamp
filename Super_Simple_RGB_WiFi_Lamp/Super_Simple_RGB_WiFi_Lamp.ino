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
#include <map>


// ############################################################# Sketch Variables #############################################################
// All variables at the top of this sketch need to be defined correctly for your light. Read the comments around each one for more details on 
// what each of them are.

#define DEFAULT_NAME "Super Simple RGB Wifi Lamp"

// Set Your Data pin - This is the pin on your ESP8266 that is connected to the LED's. Remember to add the letter "D" infront of the number 
// to map the pin correctly for your platform.
#define DATA_PIN D1

// Set the number of LED's - Simply count how many there are on your string and enter the number here.
#define NUM_LEDS 66

// Set your UTC offset - This is the time zone you are in. for example +10 for Sydney or -4 for NYC.
#define UTC_OFFSET +10

// Set the chipset and color order for the LEDs you are using. For more info on supported hardware see: https://github.com/FastLED/FastLED/wiki/Overview#supported-chipsets.
#define CHIPSET WS2812B
#define COLOR_ORDER GRB

// Limit the maximum frame rate to prevent flickering. Values around 400 or
// above cause flickering LEDs because of the WS2821 update frequency.
#define FRAME_RATE 60

// Set up LED's for each side - These arrays hold which leds are on what sides. For the basic rectangular shape in the example this relates to 4
// sides and 4 arrays. You must subract 1 off the count of the LED when entering it as the array is 0 based. For example the first LED on the 
// string is entered as 0.
int topLeds[]     = {18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
int bottomLeds[]  = {14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51};
int leftLeds[]    = {48, 49, 50};
int rightLeds[]   = {15, 16, 17};

// Eneter your wifi credentials here - If you would like to enter your wifi credentials now you can with these variables. This is a nice easy 
// method to get your ESP8266 connected to your network quickly. If you don't you can always set it up later in the wifi portal.
String SSID = "";
String Password = "";
// ########################################################## End of Sketch Variables ##########################################################

class ModeBase
{
public:
    /// Override this to initialize state specific variables
    virtual void initialize();

    // Is called once per frame to update the LEDs
    virtual void render();

    // Update config member variables based on the handed over settings
    virtual void applyConfig(JsonVariant& settings);
};

std::map<String, ModeBase*> modes;

// In some cases the automatic creation of the prototypes does not work. Do it manually...
// Config.ino
bool checkFlashConfig();
void getConfig();
bool sendConfigViaWS();
void saveConfigItem(JsonDocument& jsonSetting);
void parseConfig(JsonDocument& jsonMessage, bool sendViaWebsockets);
// LEDs.ino
void ledStringInit();
void ledModeInit();
void handleMode();
void adjustBrightnessAndSwitchMode();
// NTP.ino
void handleNTP();
bool getNTPServerIP(const char *_ntpServerName, IPAddress &_ntpServerIp);
bool sendNTPRequest();
void parseNTPResponse(uint8_t *_ntpData);
String get12hrAsString();
// Web_Server.ino
void webServerInit();
void serve404();
void servePage();
void scanForNetworks();
void updateWifiConfigTable(int _numNetworks);
void otaInit();
// Websockets.ino
void websocketsInit();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
bool websocketSend(JsonDocument& jsonMessage);
bool updateClients();
// Wifi.ino
void wifiInit();
void handleWifiConnection();
void onWifiConnected(const WiFiEventStationModeGotIP &event);
void onWifiDisconnected(const WiFiEventStationModeDisconnected &event);
void mdnsInit();

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
String  currentMode           = Mode;                                 // Placeholder variable for changing mode
String  previousMode          = "";                                   // Placeholder variable for changing mode
bool    previousState         = false;                                // Placeholder variable for changing state
float   modeChangeFadeAmount  = 0;                                    // Place holder for global brightness during mode change

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
    ledModeInit();

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

    // Handle the webserver
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
