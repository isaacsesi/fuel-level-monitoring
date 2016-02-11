 /*
 * Remote Fuel Level Monitoring System
 AUTHORS: ISAAC SESI
 CONTRIBUTORS: ABDUL-LATIF AMADU, MICHAEL AGYEI-KENA
 *
 * Reads the value of each byte of the EEPROM and prints it 
 * to the computer.
 */

 /*STEPS*
 1. Initialize system on startup
 2. Get current fuel level and GPS location 
 3. Read last saved fuel level from EEPROM 
 4. Compare the two values. If there are appreciable changes, 
 5 Call the sendAlert Function and send SMS to phone and update values on LCD.
 5. If there havent been any appreciable changes, check if any SMS has been received (Query from manager)
 6. If yes, call sendAlert function
 7. If no, check if set interval for sending alerts has timed out.
 8. If yes, call sendAlert function.
 9. If no, get fuel level and GPS location */


/*Required libraries
1. NewPing - Ultrasonic sensor library
2. GSM library
3. TInyGPS++
*/
#include <NewPing.h>
#include <LiquidCrystal.h>
#include <TinyGPSPlus.h>
#include <EEPROM.h>
#include <String.h>
#include <GSM.h>
#include <Time.h>
 
const int TRIGGER_PIN = 7;
const int ECHO_PIN  =  6;
const int ledPin =  9;
const int buzzer =  10;

const int MAX_DISTANCE = 200;
const int SAMPLE_INTERVAL = 25;
const int PING_ITERATIONS = 3;
const int UPDATE_INTERVAL = 120000;

int address = 0;
 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

long previousMillis = 0;
long interval;  
int initFuelLevel = 0;
 
void setup() {
  Serial.begin(57600);
  pinMode(ledPin, OUTPUT);      
  pinMode(buzzer, OUTPUT); 

  initFuelLevel = getFuelLevel(); 
  
}
 
void loop() {
  int initialFuelLevel = getFuelLevel();       //Get current fuel level and save as the initial fuel level
   currentFuelLevel = EEPROM.read(address);
  // Map
  interval = map(d1, 0, 200, 0, 1000);
  blinkWithoutDelay(interval);


  Serial.print(d1); Serial.print("cm    \t");
  Serial.print(interval); Serial.print("millis    \t\n");
  Serial.println();
}

int getFuelLevel() {
  int t = sonar.ping_median(PING_ITERATIONS);                 // microseconds
  int d = sonar.convert_cm(t);          // centimeters
  return d;
}






void checkAlertInterval(int interval)
{
  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   
   sendAlert();
    else
     getLevelAndLocation()
  }
}






