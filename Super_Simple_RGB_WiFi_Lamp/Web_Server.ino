void webServerInit() {
  // Set the URI's of the server
  restServer.onNotFound(serve404);
  restServer.on("/", servePage);
  restServer.begin();

  // Set up OTA on the server
  otaInit();

  // Debug
  Serial.println("[webServerInit] - Webserver was set up correctly");
}

void serve404() {
  restServer.send(404, "text/html", "");
}

void servePage() {
  // Debug
  Serial.println("[servePage] - Serving webpage");

  // Set the bool
  webSocketConnecting = true;

  restServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  restServer.sendHeader("Pragma", "no-cache");
  restServer.sendHeader("Expires", "-1");
  restServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  restServer.send(200, "text/html", "");
  restServer.sendContent_P(websiteSource);
  restServer.sendContent("");
  restServer.client().stop();

  // Scan for the networks in the area
  // scanForNetworks();
}

void scanForNetworks() {
  WiFi.scanNetworksAsync([&](int _networksFound) {
    updateWifiConfigTable(_networksFound);
  }, false);
}

void updateWifiConfigTable(int _numNetworks) {
  // Insert the RSSI into map if it is high enough (Map automatically sorts ascending)
  std::map<int32_t, int> orderedRSSI;
  for (int i = 0; i < _numNetworks; ++i) {
    if 		(i == _numNetworks)	Serial.println("[updateWifiConfigTable] - No Access Points Found");
    // else if	(WiFi.RSSI(i) < -90) Serial.println("[updateWifiConfigTable] - RSSI of " + WiFi.SSID(i) + " is too low at: " + String(WiFi.RSSI(i)));
    else {
      // Place access point reference into ordered map 
      // Serial.println("[updateWifiConfigTable] - Adding " + WiFi.SSID(i) + " to RSSI Map");
      orderedRSSI[WiFi.RSSI(i)] = i;
    }
  }
  
  // Loop through each of the found networks and put in HTML table
  String wsMessage = "{\"Wifi\":{\"ScanResults\":[";
  for(std::map<int32_t, int>::reverse_iterator  _orderedRSSI = orderedRSSI.rbegin(); _orderedRSSI != orderedRSSI.rend(); ++_orderedRSSI) {
    wsMessage += "{\"SSID\":\"" + WiFi.SSID(_orderedRSSI->second) + "\",";
    wsMessage += "\"BSSID\":\"" + WiFi.BSSIDstr(_orderedRSSI->second) + "\",";
    wsMessage += "\"CHANNEL\":\"" + String(WiFi.channel(_orderedRSSI->second)) + "\",";
    wsMessage += "\"RSSI\":\"" + String(WiFi.RSSI(_orderedRSSI->second)) + "\",";
    wsMessage += "\"ENCRYPT\":\"" + String((WiFi.encryptionType(_orderedRSSI->second) == ENC_TYPE_NONE) ? "No" : "Yes") + "\"";
    wsMessage += "}";
    wsMessage += _orderedRSSI != --orderedRSSI.rend() ? "," : "";
  }
  wsMessage += "]}}";

  // Send if the client is connected 
  if (webSocket.connectedClients(false)) webSocket.broadcastTXT(wsMessage);

  // Debug 
  Serial.println("[updateWifiConfigTable] - Number of Valid Networks Sent was: " + String(orderedRSSI.size()));
}

void otaInit() {
  OTAServer.setup(&restServer, "/update");
}
