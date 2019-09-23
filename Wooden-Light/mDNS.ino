// mDNS Methods
void mdnsInit() {
  // Create host name
  String hostName = Name != "" ? Name : DEFAULT_NAME;
  hostName.replace(" ", "-");

  // Try start the mDNS host
  if (MDNS.begin(hostName)) {
    // Debug 
    Serial.println("[startMdns] - Started MDNS responder at http://" + hostName + ".local/");

    // Add an mDNS service to the mDNS host
    if (!mdnsService) {
      mdnsService = MDNS.addService(0, "http", "tcp", 80);
      if (mdnsService) MDNS.addServiceTxt(mdnsService, "name", Name.c_str());
    }
    else Serial.println("[startMdns] - mDNS Service already started");
  }
  else Serial.println("[startMdns] - Failed to start MDNS responder as " + hostName + ".local");
}
