#pragma once
#include <ArduinoOTA.h>

// Call setupOTA() once in setup(), then call ArduinoOTA.handle() in every loop() iteration.
inline void setupOTA(const char* password) {
    ArduinoOTA.setHostname("loki-esp32");
    ArduinoOTA.setPassword(password);

    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "firmware" : "filesystem";
        Serial.println("[OTA] Start: " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\n[OTA] Done — rebooting");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        static uint8_t lastPct = 255;
        uint8_t pct = progress * 100 / total;
        if (pct != lastPct) { Serial.printf("[OTA] %u%%\r", pct); lastPct = pct; }
    });

    ArduinoOTA.onError([](ota_error_t err) {
        const char* msg = "Unknown";
        switch (err) {
            case OTA_AUTH_ERROR:    msg = "Auth failed";    break;
            case OTA_BEGIN_ERROR:   msg = "Begin failed";   break;
            case OTA_CONNECT_ERROR: msg = "Connect failed"; break;
            case OTA_RECEIVE_ERROR: msg = "Receive failed"; break;
            case OTA_END_ERROR:     msg = "End failed";     break;
        }
        Serial.printf("[OTA] Error #%u: %s\n", err, msg);
    });

    ArduinoOTA.begin();
    Serial.println("[OTA] Ready on port 3232");
}
