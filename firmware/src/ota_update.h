#pragma once
#include <ArduinoOTA.h>

inline void setupOTA(const char* password) {
    ArduinoOTA.setPassword(password);
    ArduinoOTA.onStart([]() { Serial.println("OTA start"); });
    ArduinoOTA.onEnd([]()   { Serial.println("OTA end");   });
    ArduinoOTA.begin();
}
