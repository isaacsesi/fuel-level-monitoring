#include <NewPing.h>

 
const int TRIGGER_PIN = 7;
const int ECHO_PIN  =  6;
const int ledPin =  9;
const int buzzer =  10;

const int MAX_DISTANCE = 200;
const int SAMPLE_INTERVAL = 25;
const int PING_ITERATIONS = 3;
 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int ledState = LOW;
long previousMillis = 0;
long interval;   

void setup() {
  Serial.begin(57600);
  pinMode(ledPin, OUTPUT);      
  pinMode(buzzer, OUTPUT);  
  
}
 
void loop() {
  float d1 = getDistance();                 // centimeters

  // Map
  interval = map(d1, 0, 200, 0, 1000);
  blinkWithoutDelay(interval);


  Serial.print(d1); Serial.print("cm    \t");
  Serial.print(interval); Serial.print("millis    \t\n");
  Serial.println();
}

float getDistance() {
  float t = sonar.ping_median(PING_ITERATIONS);                 // microseconds
  float d = sonar.convert_cm(t);          // centimeters
  return d;
}

void blinkWithoutDelay(int interval)
{
  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
    digitalWrite(buzzer, ledState);
  }
}


