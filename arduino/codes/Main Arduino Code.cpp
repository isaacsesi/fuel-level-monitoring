/* FINAL YEAR PROJECT
REMOTE FUEL LEVEL MONITORING SYSTEM
By 
Isaac Sesi
Abdul-Latif Amadu
Michael Adjei Kena
April 2016

LICENCE: Creative Commons, Non Commercial, Share-Alike With Attribution


How It Works

When the device is started.....
1. System measures current fuel level and compares the current fuel level with the last measured 
fuel level which has been saved in the EEPROM. If the two levels are significanly different, the system
sends a text message to the control station.

When the  device is running.....
If measurement interval time has elapsed,
1. System measures current fuel level and compares the current fuel level with the last measured 
fuel level. If the two levels are significanly different, the system
sends a text message to the control station.

2. If the two levels are not significantly different, system checks to see if it has received a 
text message requesting for the current level or position of the tanker.

3. If a text message has been received, it processes the request accordingly. 

4. If no text message has been received, system checks to see if a set reporting interval time has elapsed. If 
the reporting interval time has elapsed, the system sends a text message to the control station 
with info about the current level and position of the tanker and goes to STEP 6.

5. If time has not elapsed,

6. System measures and updates the current fuel level and stores the value in the EEPROM.

*/


#include <TinyGPS.h>
#include <LiquidCrystal.h>


//Ultrasonic Sensor
const int trigPin = 7;
const int echoPin = 6;
long duration, cm;

//GSM Module
int8_t answer;
int onModulePin= 10;
char aux_string[30];
char phone_number[]="0501359999";

//GPS Module
 float flat, flon;
 unsigned long age;

//EEPROM
 int addr = 0;

 //Intervals
const int INTERVAL = 5000;
const int FUEL_DIFFERENCE = 10;

float previousLevel = 0;
float newLevel = 0;
long previousMillis = 0;

//Create a new instance of the LCD and GPS objects
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
TinyGPS gps;

void setup(){
  Serial.begin(115200);
  r
  previousLevel = EEPROM.read(addr); 
  newLevel = getFuelLevel();

  if 

  Serial.print("Initial Fuel level is: "); Serial.println(previousLevel);
  Serial.println("-----------------------------------");
  Serial.println("");
  lcd.clear();
    lcd.print("Current Level:");
   lcd.setCursor(1,3);
   lcd.print(previousLevel);lcd.print(" litres");
}
}