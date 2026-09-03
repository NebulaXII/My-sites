#include <Arduino.h>
#include "system.h"
#include "settings.h"
#include "wifi_manager.h"
#include "web_ui.h"
#include "temperature.h"

void setup(){
  System::begin();
  Settings::begin();
  WifiManager::begin();
  WebUI::begin();
  Temperature::begin();
}

void loop(){
  System::loop();
  WifiManager::loop();
  WebUI::loop();
  Temperature::loop();
}
