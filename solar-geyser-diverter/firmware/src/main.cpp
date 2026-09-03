#include <Arduino.h>
#include "system.h"
#include "settings.h"
#include "wifi_manager.h"
#include "web_ui.h"

void setup(){
  System::begin();
  Settings::begin();
  WifiManager::begin();
  WebUI::begin();
}

void loop(){
  System::loop();
  WifiManager::loop();
  WebUI::loop();
}
