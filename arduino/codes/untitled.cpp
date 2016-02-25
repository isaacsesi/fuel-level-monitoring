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
#include <String.h>
#include <GSM.h>
#include <Time.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
 
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
int newLevel = 0;
int currentLevel;
 
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);      
  pinMode(buzzer, OUTPUT); 
  lcd.begin(16, 2);
  newLevel = getLevelAndLocation();    //Get current fuel level and save as the initial fuel level
  
}
 
void loop() {
 
   int currentLevel = EEPROM.read(address);
   if (newLevel -  currentLevel > 5)
   sendAlert();
	else if(receivedMessage() == true)
	sendAlert()
	else 
	checkAlertInterval()



  Serial.print(d1); Serial.print("cm    \t");
  Serial.print(interval); Serial.print("millis    \t\n");
  Serial.println();
}

int getFuelLevel() {
  int t = sonar.ping_median(PING_ITERATIONS);                 // microseconds
  int d = sonar.convert_cm(t);          // centimeters
  n
  return d;
}






void checkAlertInterval(int interval)
{
  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   
   sendAlert();
    else
     getLevelAndLocation();{
     	address = ++address;
     }
  }

}




/*----Function to be called when any of the following conditions are true
1. significant fuel change, 2. System has been queried    3.alert interval reached */

void sendAlert(){
  Serial.println("Preparing to send data"); 
  Serial.print("Setting Data mode...");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", 0501359999);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    {
        Serial.println("Hello, significant Change Detected.");
        Serial.println("Prevous Level = "); Serial.
        Serial.print("kW");
     Serial.println("And your bill for this month is ");
    Serial.print("GHS ");
   Serial.print(amount);
        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
            Serial.print("Sent ");    
        }
        else
        {
            Serial.print("error ");
        }
    }
    else
    {
        Serial.print("error ");
        Serial.println(answer, DEC);
    }
  
}