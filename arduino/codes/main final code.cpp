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
#include  <EEPROM.h>


//Ultrasonic Sensor
const int trigPin = 7;
const int echoPin = 6;
long duration, cm;

//GSM Module
int8_t answer;
int onModulePin= 10;
char aux_string[30];
char phone_number[]="0501370481";

//GPS Module
 float flat, flon;
 unsigned long age;

//EEPROM
unsigned int addr = 0;

 //Intervals
const int INTERVAL = 5000;
const int FUEL_DIFFERENCE = 10;

float previousLevel;
float newLevel = 0;
long previousMillis = 0;

void power_on();
float getFuelLevel();

//Create a new instance of the LCD and GPS objects
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
TinyGPS gps;

void setup(){
  Serial.begin(115200);
  lcd.begin(16, 2);
  
  //Initialize GSM Module
   lcd.print("Starting GSM...");
    power_on();
    delay(2000);


  pinMode(onModulePin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  previousLevel = EEPROM.read(addr); 
  newLevel = getFuelLevel();

  lcd.setCursor(0,1);
  lcd.print("Level: ");  lcd.print(previousLevel, 2); lcd.print(" cm^3");

  if(abs(newLevel - previousLevel >= 8)){
    sendSMS(1, newLevel);
  }
}



void loop(){
  checkFuelLevelChange();
  //checkForSystemQuery();
  checkUpdateIntervalTimeout();

}

float getFuelLevel(){

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
float  duration = pulseIn(echoPin, HIGH);
float level = duration /29 /2;

float xMin = 0.0;
float xMax = 20.6;
float yMin = 0;
float yMax = 700;
float volume = ((level - xMin)*(yMin - yMax)/(xMax-xMin) + yMax);


  return volume;
  /*
  if(level >= MIN_DISTANCE && level <= MAX_DISTANCE){
    return level;
  } else {
    return -1;
  }*/ 
}



void checkFuelLevelChange(){
  float currentLevel = getFuelLevel();
     if(abs(currentLevel - previousLevel >= FUEL_DIFFERENCE)){
      sendSMS(1, currentLevel);
    } 
      previousLevel = currentLevel;
}

/*
void checkForSystemQuery(){
  bool newQuery = false;
  while(Serial.available()){
    char userInput = Serial.read();
    if(userInput=='A' || userInput=='a')
      newQuery = true;
  }
  if(newQuery)
     sendSMS(2);
     return;
}
*/

void checkUpdateIntervalTimeout(){
  unsigned long currentMillis = millis();
 float  level = getFuelLevel();
  if(currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis; 
   
    sendSMS(3, level);
  }
  int lastMeasured = ceil(level);
  EEPROM.update(addr, lastMeasured);
}




//GSM FUNCTIONS

void sendSMS(int a, float b){
   //sets the PIN code
    sendATcommand("AT+CPIN=****", "OK", 2000);
    
    delay(3000);
    
    Serial.println("Connecting to the network...");
    lcd.setCursor(0, 1);
    lcd.println("GSM Connecting..");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.print("Setting SMS mode...");
    Serial.print("Setting SMS mode");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    { 


         lcd.clear();
        Serial.print("Level: "); lcd.print(b); lcd.print("cm3"); 
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


void power_on(){

    uint8_t answer=0;
    
    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);
    
        // waits for an answer from the module
        while(answer == 0){     // Send AT every two seconds and wait for the answer
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }
    
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialice the string
    
    delay(100);
    
    while( Serial.available() > 0) Serial.read();    // Clean the input buffer
    
    Serial.println(ATcommand);    // Send the AT command 


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(Serial.available() != 0){    
            response[x] = Serial.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
            }
        }
    // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}