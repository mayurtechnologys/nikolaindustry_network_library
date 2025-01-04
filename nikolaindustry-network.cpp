#include "NikolaIndustryNetwork.h"

NikolaIndustryNetwork::NikolaIndustryNetwork(const char* host, uint16_t port, const char* deviceId) 
    : _host(host), _port(port), _deviceId(deviceId) {}

void NikolaIndustryNetwork::begin(const char* ssid, const char* password) {
    connectToWiFi(ssid, password);
    initializeWebSocket();
}

void NikolaIndustryNetwork::loop() {
    _webSocket.loop();

    unsigned long currentMillis = millis();
    if (currentMillis - _lastPingTime > _pingInterval) {
        _webSocket.sendPing();
        _lastPingTime = currentMillis;
    }
}

void NikolaIndustryNetwork::sendMessage(const String& message) {
    _webSocket.sendTXT(message);
}

void NikolaIndustryNetwork::setEventCallback(std::function<void(const JsonObject&)> callback) {
    _eventCallback = callback;
}

void NikolaIndustryNetwork::connectToWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

void NikolaIndustryNetwork::initializeWebSocket() {
    String websocketPath = "/connect?id=" + _deviceId;
    _webSocket.beginSSL(_host, _port, websocketPath.c_str());

    _webSocket.onEvent([this](WStype_t type, uint8_t* payload, size_t length) {
        if (type == WStype_CONNECTED) {
            Serial.println("WebSocket connected!");
        } else if (type == WStype_TEXT) {
            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (error) {
                Serial.print("Error parsing JSON: ");
                Serial.println(error.c_str());
                return;
            }

            if (_eventCallback) {
                _eventCallback(doc.as<JsonObject>());
            }
        } else if (type == WStype_DISCONNECTED) {
            Serial.println("WebSocket disconnected! Reconnecting...");
            initializeWebSocket();
        }
    });
}

void NikolaIndustryNetwork::handleWebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    // Unused since we use a lambda for onEvent
}
