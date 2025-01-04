#ifndef NIKOLAINDUSTRY_NETWORK_H
#define NIKOLAINDUSTRY_NETWORK_H

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <BluetoothSerial.h>

class NikolaIndustryNetwork {
public:
    NikolaIndustryNetwork(const char* wsHost, uint16_t wsPort, const String& deviceID);

    // Initialization
    void begin();
    void loop();

    // WebSocket Messaging
    void sendMessage(const char* message);
    void sendJSON(DynamicJsonDocument& doc);

    // WiFi and AP Modes
    void connectToWiFi();
    void startAPMode(const char* apSSID, const char* apPassword);

    // Bluetooth
    void startBluetooth(const char* deviceName);

    // GPIO Control
    void controlGPIO(int pin, const char* action);

private:
    // WebSocket Event Handler
    static void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);

    // Member Variables
    String ssid;
    String password;
    String deviceID;

    const char* wsHost;
    uint16_t wsPort;
    String wsPath;

    Preferences preferences;
    WebSocketsClient webSocket;
    BluetoothSerial btSerial;

    // Helper Methods
    void loadWiFiCredentials();
    void handleWebSocketMessage(const char* message);

    unsigned long lastPingTime;
    const unsigned long pingInterval = 50000;
};

#endif
