// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[]  = "97fe2bd8-ad34-4dbf-9842-8fb31a9c1287";

const char SSID[]               = SECRET_SSID;    // Network SSID (name)
const char PASS[]               = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)
const char DEVICE_KEY[]  = SECRET_DEVICE_KEY;    // Secret device password

void onPowerChange();
void onSchedulerChange();
void onScheduler2Change();
void onButtonChange();
void onButton2Change();

CloudPower power;
CloudSchedule scheduler;
CloudSchedule scheduler_2;
bool button;
bool button_2;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(power, READWRITE, ON_CHANGE, onPowerChange);
  ArduinoCloud.addProperty(scheduler, READWRITE, ON_CHANGE, onSchedulerChange);
  ArduinoCloud.addProperty(scheduler_2, READWRITE, ON_CHANGE, onScheduler2Change);
  ArduinoCloud.addProperty(button, READWRITE, ON_CHANGE, onButtonChange);
  ArduinoCloud.addProperty(button_2, READWRITE, ON_CHANGE, onButton2Change);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);