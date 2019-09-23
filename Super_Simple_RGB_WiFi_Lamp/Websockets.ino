void websocketsInit() {
  // Start the WS Server
  webSocket.begin();

  // Set the callback for messages
  webSocket.onEvent(webSocketEvent);

  // Debug
  Serial.println("[websocketsInit] - Websocket server is now running on port " + String(81));
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED : {
      Serial.println("[webSocketEvent] - Disconnected from client number " + String(num));
    }
    break;
    case WStype_CONNECTED : {
      // Debug
      Serial.println("[webSocketEvent] - Connected to client number " + String(num) + " at " + webSocket.remoteIP(num).toString());

      // Set the boolean 
      clientNeedsUpdate = true;
    }
    break;
    case WStype_TEXT : {
      if (!processingMessage) {
        // Set the processing bool to true - drops new messages quickly
        processingMessage = true;

        // Start a JSON buffer and try parse the message
        DynamicJsonDocument jsonDocument(1024); // 536 max
        DeserializationError jsonError = deserializeJson(jsonDocument, payload);

        // if there is no error pass it to the config method
        if (jsonError) {
          Serial.print("[webSocketEvent] - Error parsing websocket message: ");
          Serial.println(jsonError.c_str());
        }
        else {
          // Debug 
          // Serial.print("Incoming Websocket message is: ");
          // serializeJson(jsonDocument, Serial);
          // Serial.println();

          // Parse config and resend
          parseConfig(jsonDocument, true);
        }
        
        // Set the processing bool to false to allow more messages
        processingMessage = false;
      }
    }
    break;
    case WStype_BIN: {
      Serial.println("[webSocketEvent] - Binary Data Not Supported");
    }
    break; 
    default : {
      Serial.println("[webSocketEvent] - Invalid WStype Used");
    }
    break;
  }
}

bool websocketSend(JsonDocument& jsonMessage) {
  // Serialize the string
  String buffer;
  serializeJson(jsonMessage, buffer); 

  // Broadcast the message
  // Serial.println("[websocketSend] - Sending: " + buffer);
  webSocket.broadcastTXT(buffer.c_str());
}

bool updateClients() {
  // Send the current values of everything to the clients when one connects
  if (clientNeedsUpdate){
    // Debug 
    Serial.println("[updateClients] - Sending updated values to clients");

    // Get and Send
    sendConfigViaWS();

    // Reset the Boolean
    clientNeedsUpdate = false;
  }
}
