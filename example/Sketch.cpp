#include "NikolaIndustryNetwork.h"

// WiFi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// NikolaIndustry Network initialization
NikolaIndustryNetwork network("nikolaindustry-network.onrender.com", 443, "your_device_id");

void setup() {
    Serial.begin(115200);
    network.begin(ssid, password);

    // Set an event callback to handle messages
    network.setEventCallback([](const JsonObject& message) {
        Serial.print("Received message: ");
        serializeJson(message, Serial);
        Serial.println();
        
        if (message["payload"]["commands"] == "control_gpio") {
            int pin = message["payload"]["pin"];
            String action = message["payload"]["actions"];

            pinMode(pin, OUTPUT);
            if (action == "HIGH") {
                digitalWrite(pin, HIGH);
            } else if (action == "LOW") {
                digitalWrite(pin, LOW);
            } else if (action == "toggle") {
                digitalWrite(pin, !digitalRead(pin));
            }

            // Send feedback
            StaticJsonDocument<256> feedbackDoc;
            feedbackDoc["status"] = digitalRead(pin) == HIGH ? "HIGH" : "LOW";
            String feedback;
            serializeJson(feedbackDoc, feedback);
            network.sendMessage(feedback);
        }
    });
}

void loop() {
    network.loop();
}
