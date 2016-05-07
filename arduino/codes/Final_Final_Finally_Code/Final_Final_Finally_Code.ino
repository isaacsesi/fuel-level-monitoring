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
const int INTERVAL = 20000;
const int FUEL_DIFFERENCE = 150;

float previousLevel;
float newLevel;
long previousMillis = 0;

void power_on();
float getFuelLevel();

//Create a new instance of the LCD and GPS objects
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
TinyGPS gps;

void setup(){
  Serial.begin(115200);
  lcd.begin(16, 2);
  pinMode(onModulePin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //Initialize GSM Module
  lcd.setCursor(0,0);
   lcd.print("Starting GSM...");
    power_on();
    delay(2000);



  
  previousLevel = EEPROM.read(addr); 
  newLevel = getFuelLevel();


  //Serial.println(previousLevel);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Last: ");
  lcd.print(previousLevel); lcd.print("ml   ");
  lcd.setCursor(0,1);
  lcd.print("New: ");
  lcd.print(newLevel); lcd.print("ml    ");

  if(abs(newLevel - previousLevel >= FUEL_DIFFERENCE)){
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



float xMin = 0.00;
float xMax = 21.50;
float yMin = 0.00;
float yMax = 949.94;
float volume = ((level - xMin)*(yMin - yMax)/(xMax-xMin) + yMax);

 
 Serial.print("level is "); Serial.println(level);
 Serial.print("volume is "); Serial.println(volume);

  return volume;
  delay(100);
}



void checkFuelLevelChange(){
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis; 
  float currentLevel = getFuelLevel();
     if(abs(currentLevel - previousLevel >= FUEL_DIFFERENCE)){
      sendSMS(1, currentLevel);
    } 
      previousLevel = currentLevel;
  }
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
 
  if(currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis; 
   float  level = getFuelLevel();
    sendSMS(3, level);
    int lastMeasured = ceil(level);
  EEPROM.update(addr, lastMeasured);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Current Level is");
  lcd.setCursor(0, 1);
  lcd.print(level); lcd.print("ml         ");
  }
  
}




//GSM FUNCTIONS

void sendSMS(int a, float b){
   //sets the PIN code
    sendATcommand("AT+CPIN=****", "OK", 2000);
    
    delay(3000);
    
    Serial.println("Connecting to the network...");
    lcd.setCursor(0, 0);
    lcd.println("GSM Connecting..");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.print("Setting SMS mode...");
    lcd.setCursor(0, 1);
    lcd.print("Setting SMS mode");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    { 


         lcd.clear();
         if(a == 1){
         Serial.println("Significant Level Change Detected");
        Serial.print("New Level is "); Serial.print(b); Serial.print(" ml"); 
         }
         else if(a == 2){
           Serial.println("Reply to Query");
        Serial.print("New Level is "); Serial.print(b); Serial.print(" ml"); 
         }

         else if(a == 3){
           Serial.println("Status Update:");
        Serial.print("Current Level is "); Serial.print(b); Serial.print(" ml"); 
         }
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
