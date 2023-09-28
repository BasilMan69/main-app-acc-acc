#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;
ESP8266WiFiMulti WiFiMulti;
SoftwareSerial receiverSerial(D1, D2);
SoftwareSerial senderSerial(D5, D6);

// const char *ssid = "American Study";
// const char *password = "66668888";
const char *ssid2 = "AmericanStudy T1";
const char *password2 = "66668888";
// const char *ssid3 = "WiFi Poop";
// const char *password3 = "CircularShit";

const char *serverAddress = "automaticcropcaretaker.com";
const int serverPort = 80;
const char *serverURL = "/ws";
const char *serverProtocol = "arduino";

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  // WiFiMulti.addAP(ssid, password);
  WiFiMulti.addAP(ssid2, password2);
  // WiFiMulti.addAP(ssid3, password3);

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.println("[SETUP] Connecting to WiFi...");
  }

  Serial.print("[SETUP] WiFi Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void setupWebSocket() {
  Serial.println("[SETUP] Initializing WebSocket...");
  webSocket.begin(serverAddress, serverPort, serverURL, serverProtocol);
  webSocket.onEvent(webSocketEvent);
}

void setup() {
  Serial.begin(9600);
  receiverSerial.begin(9600);
  senderSerial.begin(9600);
  Serial.println("\n[SETUP] BOOT");
  setupWiFi();
  setupWebSocket();
}

void loop() {
  webSocket.loop();
  if (receiverSerial.available()) {
    receiveDataAndSendToServer();
  }
}

void sendDataToServer(String data) {
  webSocket.sendTXT(data);
}

void receiveDataAndSendToServer() {
  String data = receiverSerial.readStringUntil('\n');  // Read the JSON string from Serial
  Serial.println(data);
  // Check if the received data starts with "Sensors_data:"
  if (data.startsWith("Sensors_data:")) {
    // Extract the JSON part from the input string
    String jsonData = data.substring(data.indexOf('{'));
    Serial.print(jsonData);
    sendDataToServer(jsonData);
  }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      break;
    case WStype_TEXT:
      Serial.print("Received message: ");
      String data = (char *)payload;
      Serial.println(data);
      //"{"type":"control/both_motors"}" <- This is sample data from server
      if(data.substring(10).startsWith("control")){
        senderSerial.println(data);
      }
      break;
  }
}