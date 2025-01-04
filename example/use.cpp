#include "nikolaindustry-network.h"

NikolaIndustryNetwork network("nikolaindustry-network.onrender.com", 443, "your_device_id");

void setup() {
    Serial.begin(115200);
    network.begin();
}

void loop() {
    network.loop();
}
