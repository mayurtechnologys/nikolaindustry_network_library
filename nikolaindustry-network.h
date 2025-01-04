#ifndef NIKOLA_INDUSTRY_NETWORK_H
#define NIKOLA_INDUSTRY_NETWORK_H

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

class NikolaIndustryNetwork {
public:
    NikolaIndustryNetwork(const char* host, uint16_t port, const char* deviceId);

    void begin(const char* ssid, const char* password); // Initialize WiFi and WebSocket
    void loop();                                        // Keep the WebSocket alive
    void sendMessage(const String& message);           // Send a message
    void setEventCallback(std::function<void(const JsonObject&)> callback);

private:
    const char* _host;
    uint16_t _port;
    String _deviceId;
    WebSocketsClient _webSocket;

    unsigned long _lastPingTime = 0;
    const unsigned long _pingInterval = 50000;  // 50 seconds
    std::function<void(const JsonObject&)> _eventCallback;

    void connectToWiFi(const char* ssid, const char* password);
    void initializeWebSocket();
    static void handleWebSocketEvent(WStype_t type, uint8_t* payload, size_t length);
};

#endif
