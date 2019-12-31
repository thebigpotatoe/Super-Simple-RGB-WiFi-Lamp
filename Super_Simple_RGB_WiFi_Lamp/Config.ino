// Check if the flash size set in the IDE is the same as the onboard chip
bool checkFlashConfig() {
  //  Set bool pesimistically 
  bool flashSizeCorrect = false;

  // Get actual flash size and size set in IDE
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();

  // Compare the two
  if (realSize >= ideSize) {
    // Get info about the SPIFFS
    if (SPIFFS.begin()) {
      FSInfo fs_info;
      SPIFFS.info(fs_info);
      if (fs_info.totalBytes != 0) {
        // Change the boolean to true if the config is ok
        flashSizeCorrect = true;
      }
      else {
        Serial.println("[checkFlashConfig] - SPIFFS size was set to 0, please select a SPIFFS size from the \"tools->flash size:\" menu");
      }
    }
    else {
      Serial.println("[checkFlashConfig] - SPIFFS size was set to 0, please select a SPIFFS size from the \"tools->flash size:\" menu");
    }
  }
  else {
    // Tell the user the flash is incorrect if it is not
    Serial.println("[checkFlashConfig] - Flash chip set to the incorrect size, correct size is; " + String(realSize));
  }

  // Return the boolean
  return flashSizeCorrect;
}

// Get the config file from the flash chip
void getConfig() {
  // Check if flash is configured correctly
  if (checkFlashConfig()) {
    // Mount the file system
    if (SPIFFS.begin()) {
      // Check if the file exists
      if (SPIFFS.exists("/DeviceConfig.json")) {
        // Open file in read only mode and check if it opened correctly
        File deviceConfigFile = SPIFFS.open("/DeviceConfig.json", "r");
        if (deviceConfigFile) {
          // Get size of file and allocate memory
          size_t size = deviceConfigFile.size();
          char filebuffer[size];
          deviceConfigFile.readBytes(filebuffer, size);

          // Create JSON buffer and parse file
          DynamicJsonDocument jsonDocument(1024); 
          DeserializationError jsonError = deserializeJson(jsonDocument, filebuffer);

          // Check if file parsed correctly and decode
          if (!jsonError) {
            parseConfig(jsonDocument);
          }
          else {
            Serial.print("[getConfig] - deserializeJson() failed: ");
            Serial.println(jsonError.c_str());
          }
        }
        else {
          Serial.println("[getConfig] - Failed to open device config file");
        }
      }
      else {
        // Create a new config file if none exists and pre populate
        Serial.println("[getConfig] - No Device Config file found, creating new one");
        DynamicJsonDocument jsonDocument(1024); 
        jsonDocument["Name"] = Name;
        jsonDocument["Mode"] = Mode;
        jsonDocument["State"] = State;
        jsonDocument["Wifi"]["SSID"] = programmedSSID;
        jsonDocument["Wifi"]["Password"] = programmedPassword;
        saveConfigItem(jsonDocument);
      }
    }
    else Serial.println("[getConfig] - Failed to mount FS");
  }
  else Serial.println("[getConfig] - Could not get parameters due to incorrect IDE flash settings");
}

bool sendConfigViaWS() {
    // Check if flash is configured correctly
  if (checkFlashConfig()) {
    // Mount the file system
    if (SPIFFS.begin()) {
      // Check if the file exists
      if (SPIFFS.exists("/DeviceConfig.json")) {
        // Open file in read only mode and check if it opened correctly
        File deviceConfigFile = SPIFFS.open("/DeviceConfig.json", "r");
        if (deviceConfigFile) {
          // Get size of file and allocate memory
          size_t size = deviceConfigFile.size();
          char filebuffer[size];
          deviceConfigFile.readBytes(filebuffer, size);

          // Parse the file
          DynamicJsonDocument jsonDocument(1024); 
          DeserializationError jsonError = deserializeJson(jsonDocument, filebuffer);

          // Check if file parsed correctly and decode
          if (!jsonError) {
            parseConfig(jsonDocument);

            // Send the updated config back to the clients via websocket
            websocketSend(jsonDocument);

            return true;
          }
          else {
            Serial.print("[getConfigAsDocument] - deserializeJson() failed: ");
            Serial.println(jsonError.c_str());
          }
        }
        else Serial.println("[getConfigAsDocument] - Failed to open device config file");
      }
      else Serial.println("[getConfigAsDocument] - Device config file does not exist");
    }
    else Serial.println("[getConfigAsDocument] - Failed to mount FS");
  }
  else Serial.println("[getConfigAsDocument] - Could not get parameters due to incorrect IDE flash settings");

  return false;
}

// Save a specific config item parsed to JSON
void saveConfigItem(JsonDocument& jsonSetting) {
  // Debug
  // Serial.print("[saveConfigItem] - Incoming Document is: ");
  // serializeJson(jsonSetting, Serial);
  // Serial.println();

  // Check if the flash is configured correctly
  if (checkFlashConfig()) {
    // Mount the file system
    if (SPIFFS.begin()) {
      // Start a json buffer
      String stringBuffer;
      DynamicJsonDocument currentjsonDocument(1024); 
      File deviceConfigFile;

      // Read the contents of the current file
      if (SPIFFS.exists("/DeviceConfig.json")) {
        deviceConfigFile = SPIFFS.open("/DeviceConfig.json", "r");
        if (deviceConfigFile) {
          // Get size of file and allocate memory
          size_t size = deviceConfigFile.size();
          char filebuffer[size];
          deviceConfigFile.readBytes(filebuffer, size);

          // Create JSON buffer and parse file
          DeserializationError jsonError = deserializeJson(currentjsonDocument, filebuffer);
          if (!jsonError) {
            serializeJson(currentjsonDocument, stringBuffer);
            // Serial.print("Stored Document is currently: ");
            // serializeJson(currentjsonDocument, Serial);
            // Serial.println();
          }
          else {
            Serial.print(F("[saveConfigItem] - Deserialize Json failed: "));
            Serial.println(jsonError.c_str());
          }

          // Close the file for reading
          deviceConfigFile.close();
        }
      }
      else Serial.println("[saveConfigItem] - No Device Config file found");

      // Modify and write the updated contents back to file
      deviceConfigFile = SPIFFS.open("/DeviceConfig.json", "w");
      if (deviceConfigFile) {
        // Deserilise json from string
        if (stringBuffer != "") DeserializationError jsonError = deserializeJson(currentjsonDocument, stringBuffer);

        // Put all keys from the new object into the old settings object - will overide existing values
        for (auto kvp : jsonSetting.as<JsonObject>()) { 
          currentjsonDocument[kvp.key()] = kvp.value();
        }

        // Debug
        // Serial.print("[saveConfigItem] - Stored Document is now: ");
        // serializeJson(currentjsonDocument, Serial);
        // Serial.println();

        // Print updated object to file and close
        serializeJson(currentjsonDocument, deviceConfigFile);
        deviceConfigFile.close();
      }

      // Debug
      // Serial.println("[saveConfigItem] - Device config saved");
    }
    else
      Serial.println("[saveConfigItem] - Failed to mount FS");
  }
  else
    Serial.println("[saveConfigItem] - Could not set config due to incorrect IDE flash settings");
}

// Generic message parser
void parseConfig(JsonDocument& jsonMessage) {
  // Config Parameters
  /*
  {
    "Name": "Test",
    "Mode": "Colour",
    "State": true,
    "Fade Period" : 1,
    "Colour": {
      "Red": 0,
      "Green": 0,
      "Blue": 0
    },
    "Rainbow": {
      "Hue": 0,
      "Speed": 10,
      "Brightness": 100
    },
    "Clock": {
      "Epoch" : 24356789,
      "hourColour": {
        "Red": 0,
        "Green": 0,
        "Blue": 0
      },
      "minuteColour": {
        "Red": 0,
        "Green": 0,
        "Blue": 0
      }
    },
    "Bell Curve" : {
      "Red": 0,
      "Green": 0,
      "Blue": 0
    },
    "Night Rider" : {
    },
    "Circle" : {
    },
	"Sparkle" : {
      "Red": 0,
      "Green": 0,
      "Blue": 0
      "Speed": 30,
    },
    "Color Wipe" : {
      "Red": 0,
      "Green": 0,
      "Blue": 0
      "Speed": 20,
    },
    "Confetti" : {
      "Red": 0,
      "Green": 0,
      "Blue": 0
      "Speed": 100,
    },
    "Visualiser" : {
      "Period" : 250,
      "MinThreshold" : 100,
      "MaxThreshold" : 750,
      "FadeUp" : 32,
      "FadeDown" : 32,
      "HueOffset" : 170,
    },
    "Wifi": {
      "SSID": "Test",
      "Password": "Test"
    }
  }
  */  

  // Debug
  // Serial.println();
  // Serial.print("[parseConfig] - Incoming JSON document is: "); 
  // serializeJson(jsonMessage, Serial);
  // Serial.println();

  // Get the root object
  JsonObject jsonSettingsObject = jsonMessage.as<JsonObject>();

  // Check for Name, Mode, and State
  jsonSettingsObject["Name"] = Name = (Name != "") ? jsonSettingsObject["Name"] | Name : DEFAULT_NAME;
  jsonSettingsObject["Mode"] = Mode = jsonSettingsObject["Mode"] | Mode;
  jsonSettingsObject["State"] = State = jsonSettingsObject["State"] | State;
  jsonSettingsObject["Fade Time"] = FadeTime = jsonSettingsObject["Fade Time"] | FadeTime;
  
  // Might need to reconnect wifi with Name change

  // Check for Wifi Settings
  JsonVariant wifiSettings = jsonSettingsObject["Wifi"];
  if (wifiSettings) {
      String pssidBuffer = wifiSettings["PSSID"] | programmedSSID;
      if (pssidBuffer == programmedSSID && wifiSettings.containsKey("SSID")) {
        String ssidBuffer = wifiSettings["SSID"] | SSID;
        String passBuffer = wifiSettings["Password"] | Password;

        wifiSettings["SSID"] = SSID = (SSID != ssidBuffer) ? ssidBuffer : SSID;
        wifiSettings["Password"] = Password = (Password != passBuffer) ? passBuffer : Password;
        wifiSettings["PSSID"] = programmedSSID;
      }
      else if (pssidBuffer != programmedSSID) {
        wifiSettings["SSID"] = SSID = programmedSSID;
        wifiSettings["Password"] = Password = programmedPassword;
        wifiSettings["PSSID"] = programmedSSID;
      }

      if (wifiSettings.containsKey("Rescan")) {
        // Scan and wifi here
        scanForNetworks();

        // Remove the object to not store it
        wifiSettings.remove("Rescan");
      }
  }

  // Apply settings to the modes
  for(auto it = modes.begin(); it != modes.end(); it++) {
    JsonVariant settings = jsonSettingsObject[it->first];
    if (settings) {
        it->second->applyConfig(settings);
    }
  }

  // Debug
  // Serial.print("[parseConfig] - Final JSON document is: "); 
  // serializeJson(jsonMessage, Serial);
  // Serial.println();

  // Save the config
  saveConfigItem(jsonMessage);
}
