void handleNTP() {
  // Change the bool after a waiting period
  if ( millis() - lastNTPCollectionTime > collectionPeriod ) ntpTimeSet = false;

  // Check if the time needs to be updated 
  if (!ntpTimeSet) {

    // Collect the current time if the wifi is connected
    if (WiFi.isConnected()) {

      // Get the IP Address of the nearest server
      if (getNTPServerIP(ntpHostName.c_str(), ntpIpAddress)) {
          sendNTPRequest();
      }
    }
  }
}
bool getNTPServerIP(const char *_ntpServerName, IPAddress &_ntpServerIp) {
  // Probe the DNS for the IP Address of the NTP Server
  if (!WiFi.hostByName(_ntpServerName, _ntpServerIp)) {
    Serial.println("[getNTPServerIP] - Failed to lookup DNS results for NTP server \"" + String(_ntpServerName) + "\"");
    return false;
  }
  else {
    // Serial.println("[getNTPServerIP] - DNS results for \"" + String(_ntpServerName) + "\" was " + _ntpServerIp.toString());
    return true;
  }
}
bool sendNTPRequest() {
  // Attempt to connect to NTP Server
  if (udpClient.connect(ntpIpAddress, 123)) {
    // Debug
    // Serial.println("[sendNTPRequest] - Connected to NTP server");

    // Initialize values needed to form NTP request
    byte packetBuffer[48];
    memset(packetBuffer, 0, 48);
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // Send packet to NTP server and setup response handler
    if (udpClient.write(packetBuffer, 48)) {
      // Debug
      // Serial.println("[sendNTPRequest] - Message sent to NTP server");

      // Create a handler for the received packet
      udpClient.onPacket([&](AsyncUDPPacket packet) {
        parseNTPResponse(packet.data());
      });

      // Set the delay to 1000 milliseconds in case the request fails
      collectionPeriod = 1000;
      lastNTPCollectionTime = millis();
      ntpTimeSet = true;

      // Return true on send message
      return true;
    }
    else Serial.println("[sendNTPRequest] - Message was not sent to NTP Server");
  }
  else Serial.println("[sendNTPRequest] - Could not connect to NTP Server");

  // Always return
  return false;
}
void parseNTPResponse(uint8_t *_ntpData) {
  // Convert data to a unix time
  unsigned long highWord = word(_ntpData[40], _ntpData[41]);
  unsigned long lowWord = word(_ntpData[42], _ntpData[43]);
  currentEpochTime = highWord << 16 | lowWord;
  currentEpochTime += utcOffset - 2208988800UL;

  // Set time in library
  setTime(currentEpochTime);

  // Set the timer back to the desired wait period
  lastNTPCollectionTime = millis();
  collectionPeriod = 3600 * 1000;
  ntpTimeSet = true;

  // Close the connection
  udpClient.close();

  // Debug
  Serial.println("[getTime] - Current time set to: " + get12hrAsString());
}
String get12hrAsString() {
  String hrStr = hourFormat12() < 10 ? "0" + String(hourFormat12()) : String(hourFormat12());
  String minStr = minute() < 10 ? "0" + String(minute()) : String(minute());
  String secStr = second() < 10 ? "0" + String(second()) : String(second());
  return hrStr + ":" + minStr + ":" + secStr + String(isAM() ? "AM" : "PM");
}
