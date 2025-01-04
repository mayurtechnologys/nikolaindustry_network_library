#include "nikolaindustry-network.h"

// Constructor
NikolaIndustryNetwork::NikolaIndustryNetwork(const char* wsHost, uint16_t wsPort, const String& deviceID)
    : wsHost(wsHost), wsPort(wsPort), deviceID(deviceID), lastPingTime(0) {
    wsPath = "/connect?id=" + deviceID;
}

// Initialize the library
void NikolaIndustryNetwork::begin() {
    preferences.begin("nikolaPrefs", false);
    loadWiFiCredentials();
    connectToWiFi();
    webSocket.beginSSL(wsHost, wsPort, wsPath.c_str());
    webSocket.onEvent(webSocketEvent);
}

// Main loop
void NikolaIndustryNetwork::loop() {
    webSocket.loop();

    unsigned long currentMillis = millis();
    if (currentMillis - lastPingTime > pingInterval) {
        webSocket.sendPing();
        lastPingTime = currentMillis;
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected! Reconnecting...");
        connectToWiFi();
    }
}

// Load WiFi credentials
void NikolaIndustryNetwork::loadWiFiCredentials() {
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("pass", "");

    if (ssid.isEmpty() || password.isEmpty()) {
        Serial.println("No saved WiFi credentials found!");
    }
}

// Connect to WiFi
void NikolaIndustryNetwork::connectToWiFi() {
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

// Start AP Mode
void NikolaIndustryNetwork::startAPMode(const char* apSSID, const char* apPassword) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPassword);
    Serial.println("Started AP Mode");
}

// Start Bluetooth Configuration
void NikolaIndustryNetwork::startBluetooth(const char* deviceName) {
    btSerial.begin(deviceName);
    while (btSerial.available()) {
        String data = btSerial.readString();
        Serial.println("Bluetooth Data: " + data);
    }
}

// WebSocket Event Handler
void NikolaIndustryNetwork::webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    if (type == WStype_CONNECTED) {
        Serial.println("WebSocket Connected");
    } else if (type == WStype_TEXT) {
        handleWebSocketMessage((const char*)payload);
    } else if (type == WStype_DISCONNECTED) {
        Serial.println("WebSocket Disconnected. Reconnecting...");
    }
}

// Handle WebSocket Message
void NikolaIndustryNetwork::handleWebSocketMessage(const char* message) {
    StaticJsonDocument<1024> doc;
    if (deserializeJson(doc, message) == DeserializationError::Ok) {
        const char* command = doc["payload"]["commands"];
        if (strcmp(command, "control_gpio") == 0) {
            int pin = doc["payload"]["pin"];
            const char* action = doc["payload"]["actions"];
            controlGPIO(pin, action);
        }
    } else {
        Serial.println("Invalid JSON");
    }
}

// GPIO Control
void NikolaIndustryNetwork::controlGPIO(int pin, const char* action) {
    pinMode(pin, OUTPUT);
    if (strcmp(action, "HIGH") == 0) {
        digitalWrite(pin, HIGH);
    } else if (strcmp(action, "LOW") == 0) {
        digitalWrite(pin, LOW);
    }
}

// Send a message
void NikolaIndustryNetwork::sendMessage(const char* message) {
    webSocket.sendTXT(message);
}

// Send JSON message
void NikolaIndustryNetwork::sendJSON(DynamicJsonDocument& doc) {
    String jsonString;
    serializeJson(doc, jsonString);
    webSocket.sendTXT(jsonString.c_str());
}
