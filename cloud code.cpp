#include <Arduino.h>
#include "thingProperties.h"
#include "DHT.h"
#include <Wire.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

#define DPIN 15  //Pin to connect DHT sensor (GPIO number)
#define DTYPE DHT11 // Define DHT 11 or DHT22 sensor type

int current_timer1 = 0, current_timer2 = 0;
const int sensorIn1 = 35; //current sensor 1
const int sensorIn2 = 34; //current sensor 2
const long eventTime_temp = 5000;
const long eventTime_current = 2000;
//const long autocutoff = 10000;
int mVperAmp = 185;
double Voltage1 = 0;
double Voltage2 = 0;
double VRMS1 = 0;
double VRMS2 = 0;
double AmpsRMS1 = 0;
double AmpsRMS2 = 0;
double Amps1 = 0;
double Amps2 = 0;
double combinedPower = 0;
const float a = 0.0254;
const float b = -0.0830;
const float c = 0.8635;
const float d = -0.1036;
bool forcestop1 = false;
bool forcestop2 = false;
int counter = 0;
int counter_2 = 0;
unsigned long previousTime_temp = 0;
unsigned long previousTime_current = 0;


DHT dht(DPIN, DTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);



void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.print("Smart Multiplug"); 
  delay(3000); // Display for 3 seconds
  lcd.clear();

  pinMode(18, OUTPUT); //pin for relay 2
  pinMode(4, OUTPUT);  // pin for relay 1
  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you�ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
 setDebugMessageLevel(2);
 ArduinoCloud.printDebugInfo();
}

 void loop() {
  ArduinoCloud.update();
  
  
  unsigned long currentTime = millis();

  if (currentTime - previousTime_temp >= eventTime_temp )
  {
    float tc = dht.readTemperature();
    lcd.setCursor(0, 1);
    lcd.println("Temp:");
    lcd.print(tc);
    lcd.print("C");
    Serial.print("Temp:");
    Serial.println(tc);
    if(tc>30)
  {
    button = LOW;
    forcestop1 = true;  // Modify scheduler property
    digitalWrite(4, LOW);
    button_2 = LOW;
    forcestop2 = true;  // Modify scheduler_2 property
    digitalWrite(18, LOW);
    lcd.clear();
    lcd.setCursor(0,2);
    lcd.print("!!CAUTION!!");
    lcd.setCursor(0,1);
    lcd.print("!Temp exceeded!!");
    delay(4000);
  }
    previousTime_temp = currentTime;
  }


  Voltage1 = getVPP(sensorIn1);
  VRMS1 = (Voltage1 / 2.0) * 0.707;
  Amps1 = ((VRMS1 * 1000) / mVperAmp);
  AmpsRMS1 = ((a * Amps1 * Amps1 * Amps1) + (b * Amps1 * Amps1) + (c * Amps1) + d);
  //Serial.print(current_1);
  Voltage2 = getVPP(sensorIn2);
  VRMS2 = (Voltage2 / 2.0) * 0.707;
  Amps2 = ((VRMS2 * 1000) / mVperAmp);
  AmpsRMS2 = ((a * Amps2 * Amps2 * Amps2) + (b * Amps2 * Amps2) + (c * Amps2) + d);
  //where a = 0.0254; b = -0.0830; c = 0.8635; d = -0.1036;
  if (currentTime - previousTime_current >= eventTime_current){
    if(AmpsRMS1 < 0.08){
      AmpsRMS1 = 0;
      }
    if(AmpsRMS2 < 0.08){
      AmpsRMS2 = 0;
      }
  lcd.setCursor(0, 0);
  lcd.print("C1:");
  lcd.print(AmpsRMS1);
  lcd.print("A");
  
   //lcd.setCursor(0, 1);
  lcd.print("C2:");
  lcd.print(AmpsRMS2);
  lcd.print("A");

  combinedPower = (AmpsRMS1 + AmpsRMS2) * 220;
  Serial.println(AmpsRMS1);
  Serial.println(AmpsRMS2);
  power = combinedPower;

  previousTime_current = currentTime;

  }
  //Relay auto off
  if (AmpsRMS1 <= 0 && button == HIGH) {
    current_timer1++;
    if (current_timer1 == 20) {
        button = LOW;
        forcestop1 = true; // Modify scheduler property
        digitalWrite(4, LOW);
        current_timer1 = 0;
    }
    if (AmpsRMS1 > 0) {
        current_timer1 = 0;
        //forcestop1 = false;
    }
    delay(500);
}
if (AmpsRMS2 <= 0 && button_2 == HIGH) {

    current_timer2++;
    if (current_timer2 == 20) {
        button_2 = LOW;
        forcestop2 = true;
      //scheduler_2.isActive(false); // Modify scheduler_2 property // Modify scheduler_2 property
        digitalWrite(18, LOW);
        current_timer2 = 0;
    }

    if (AmpsRMS2 > 0) {
        current_timer2 = 0;
       // forcestop2 = false;
    }
    delay(500);
}

if (AmpsRMS1 > 2) {
    button = LOW;
    forcestop1 = true;  // Modify scheduler property
    digitalWrite(4, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("!!CAUTION!!");
    lcd.setCursor(0,1);
    lcd.print("Current1 exceeded");
    delay(2000);
    
}

if (AmpsRMS2 > 2) {
    button_2 = LOW;
    forcestop2 = true;  // Modify scheduler_2 property
    digitalWrite(18, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("!!CAUTION!!");
    lcd.setCursor(0,1);
    lcd.print("Current2 exceeded");
    delay(2000);

  }
  
  if (button == LOW)
  {
    current_timer1 = 0;
    forcestop1 = false;
  }
  
  if (button_2 == LOW)
  {
    current_timer2 = 0;
    forcestop2 = false; 
  }


  handleButtonAndScheduler(button, scheduler, 4, counter, forcestop1);
  handleButtonAndScheduler(button_2, scheduler_2, 18, counter_2,forcestop2);

}


void handleButtonAndScheduler(bool &button, CloudSchedule &scheduler, int pin, int &counter, bool &forcestop) {
  int schedulerstatus = scheduler.isActive();
   if (counter == 1) {
     if(button == LOW || forcestop){
       schedulerstatus = 0;
     }
     else if (!schedulerstatus ) {
      button = LOW;
      counter = 0;
    }
  }

  if (schedulerstatus && button == HIGH && !forcestop) {
    digitalWrite(pin, HIGH);
    counter = 1;
  } else if (!schedulerstatus && button == HIGH && !forcestop) {
    digitalWrite(pin, HIGH);
  } else if (schedulerstatus && button == LOW && !forcestop) {
    button = HIGH;
    digitalWrite(pin, HIGH);
    counter = 1;
  } else {
    digitalWrite(pin, LOW);
  }
}


float getVPP(int sensorPin) {
  int readValue;
  int maxValue = 0;
  int minValue = 4096;

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) {
    readValue = analogRead(sensorPin);
    if (readValue > maxValue) {
      maxValue = readValue;
    }
    if (readValue < minValue) {
      minValue = readValue;
    }
  }

  float result = ((maxValue - minValue) * 3.3) / 4096.0;
  return result;
}

/*
  Since Button is READ_WRITE variable, onButtonChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onButtonChange()  {
  // Add your code here to act upon Button change
   if (button == HIGH) {
    digitalWrite(4, HIGH);
  }
}

/*
  Since Button2 is READ_WRITE variable, onButton2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onButton2Change()  {
  // Add your code here to act upon Button2 change
    if (button_2 == HIGH) {
    digitalWrite(18, HIGH);
  }
}

/*
  Since Scheduler is READ_WRITE variable, onSchedulerChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onSchedulerChange()  {
  // Add your code here to act upon Scheduler change
}

/*
  Since Scheduler2 is READ_WRITE variable, onScheduler2Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onScheduler2Change()  {
  // Add your code here to act upon Scheduler2 change
}

/*
  Since Power is READ_WRITE variable, onPowerChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onPowerChange()  {
  //combinedPower = (AmpsRMS1 + AmpsRMS2) * 220 ;
  //power = AmpsRMS;
}


    
