#include <NewPing.h>
#include <LiquidCrystal.h>
 
const int TRIGGER_PIN = 7;
const int ECHO_PIN  =  6;

const int MAX_DISTANCE = 300;
const int SAMPLE_INTERVAL = 25;
const int PING_ITERATIONS = 3;
 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
 

int interval;   

void setup() {
  Serial.begin(9600);
  lcd.begin(20,4);
  float currentLevel = getFuelLevel() ;
Serial.print("The current fuel level is "); Serial.println(currentLevel);
delay(1000);
}
 
void loop() {
  float level = getFuelLevel(); 
  Serial.print(level); Serial.println( "cm  \t");  
  delay(1000);
  //lcd.print(level); lcd.print("cm \t");
}

float getFuelLevel() {
    float t = sonar.ping_median(PING_ITERATIONS);                 // microseconds
  float d = sonar.convert_cm(t);          // centimeters                 // centimeters
  return d;
}

/*
void blinkWithoutDelay(int interval)
{
  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

  }
}
*/

