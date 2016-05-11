
//Include libraries and dependencies
#include <LiquidCrystal.h>
#include  <EEPROM.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//Ultrasonic Sensor initializations
const int trigPin = 7;
const int echoPin = 6;


//GSM Module initializations
int8_t answer;
int onModulePin= 10;
char aux_string[30];
char phone_number[]="0503064768";

//GPS Module initializations
static const int RXPin = 9, TXPin = 8;
static const uint32_t GPSBaud = 9600;
float latitude;
float longitude;


//EEPROM
unsigned int addr = 0;

 //Intervals
const int INTERVAL = 20000;
const int FUEL_DIFFERENCE = 100;

//Other Variables
float previousLevel;
float newLevel;
long previousMillis = 0;
char inchar;

//Function declarations
void power_on();
float getFuelLevel();
void setupGPS();
void getGPSInfo();
void  checkForSystemQuery();
void setupGSMReceiveText();
void sendSMS(int a, float b, float c, float d);
float getLatitude();
float getLongitude();
  void checkFuelLevelChange();
  void checkUpdateIntervalTimeout();

//Create a new instance of the LCD and GPS objects
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial gpsModule(RXPin, TXPin);

void setup(){
  gpsModule.begin(GPSBaud);
  Serial.begin(57600);
  lcd.begin(16, 2);
  pinMode(onModulePin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


   //SETUP GSM MODULE
  power_on();
  delay(2000);

   setupGSMReceiveText();

   //SETUP GPS MODULE
   setupGPS();
   delay(1000);
  
  //READ THE LAST SAVED FUEL LEVEL AND MEASURE THE NEW LEVEL
  previousLevel = EEPROM.read(addr); 
 newLevel = getFuelLevel();

//PRINT OUT THE OLD LEVEL ON THE SERIAL MONITOR
 Serial.print("Previous Fuel Level is ");
 Serial.println(previousLevel);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Last: ");
  lcd.print(previousLevel); lcd.print("ml   ");
  lcd.setCursor(0,1);
  lcd.print("New: ");
  lcd.print(newLevel); lcd.print("ml    ");

}




/*---------------------------------------------------
LOOP FUNCTIONS
----------------------------------------------------*/
void loop(){
  checkFuelLevelChange();
  checkForSystemQuery();
  checkUpdateIntervalTimeout();
}







/*---------------------------------------------------
ULTRASONIC SENSOR (FUEL MEASURING) FUNCTIONS
----------------------------------------------------*/

//THIS FUNCTION MEASURES THE HEIGHT OF EMPTY SPACE FROM THE SURFACE OF THE FUEL TO THE TOP 
//OF THE TANKER. IT THEN CALCULATES THE VOLUME OF FUEL LEFT IN THE TANKER USING A SIMPLE 
//LINEAR MAPPING RELATION. TOTAL VOLUME AND HEIGHT OF THE TANK ARE ALREADY KNOWN
float getFuelLevel(){

//Send Ultasonic pulses
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 

  //Calculates the height of empty space
float  duration = pulseIn(echoPin, HIGH);
float level = duration /29 /2;

//This confusing piece of code maps the height of empty space measured by the ultrasonic sensor
//to a corresponding volume. This volume is subtracted from the total volume of the container to 
//get the volume of the rest of the liquid in the container

//Mapping Relation
float xMin = 0.00;
float xMax = 21.50;  //height or Pringles container
float yMin = 0.00;
float yMax = 949.94; // volume of Pringles container. Since the Pringles container is a perfect cylinder, its volume was obtained using pi * r^2 * h
float volume = ((level - xMin)*(yMin - yMax)/(xMax-xMin) + yMax);   //calculate the volume left from a given height

 
 Serial.print("height is "); Serial.println(level);
 Serial.print("volume is "); Serial.println(volume);

  return volume;
  delay(100);
}





/*---------------------------------------------------
 LOOPING FUNCTIONS DEFINITIONS
----------------------------------------------------*/


 void checkFuelLevelChange(){
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis; 
  float currentLevel = getFuelLevel();
     if(abs(currentLevel - previousLevel >= FUEL_DIFFERENCE)){
        lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Significant ");
       lcd.setCursor(0,1);
      lcd.print("Change Detected  ");
        float  level = getFuelLevel();
    float  lati = getLatitude();
    float longi = getLongitude();
    sendSMS(1, level,  6.670980, -1.561330);
    } 
      previousLevel = currentLevel;
  }
}



//CHECK IF THE USER HAS SENT AN SMS QUERY TO THE SYSTEM
void  checkForSystemQuery(){
  if(Serial.available() > 0)
  {
    inchar=Serial.read(); //Get the character from the cellular serial port.
    Serial.print(inchar); //Print the incoming character to the terminal.
      if(inchar== 'X'){
           lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Query Received");
        delay(1000);
         lcd.setCursor(0,1);
        lcd.print("Sending Response");
        delay(1000);
    float  level = getFuelLevel();
    float  lati = getLatitude();
    float longi = getLongitude();
    sendSMS(2, level,  6.670980, -1.561330);
  }
}
      else {
        //DO domething
      }         
}



//CHECK IF THE SYSTEM HAS TIMED OUT. EVERY 30 SECONDS THE SYSTEM IS SUPPOSED TO SEND
// A STATUS REPORT TO THE USER
void checkUpdateIntervalTimeout(){
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis; 
     float  level = getFuelLevel();
    float  lati = getLatitude();
    float longi = getLongitude();
     lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("System Timeout");
  lcd.setCursor(0, 1);
  lcd.print("Sending Update..");
    sendSMS(3, level, lati, longi);
    int lastMeasured = ceil(level);
  EEPROM.update(addr, lastMeasured);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Current Level is");
  lcd.setCursor(0, 1);
  lcd.print(level); lcd.print("ml         ");
  }
  
}






/*---------------------------------------------------
  GPS MODULE FUNCTIONS
----------------------------------------------------*/

//SET THE GPS MODULE UP
void setupGPS(){
  // This sketch displays information every time a new sentence is correctly encoded.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Setting up GPS..");
  delay(3000);

// Check to see if there is data coming from the GPS and read it
  if (gpsModule.available() > 0)
    if (gps.encode(gpsModule.read()))
    lcd.clear();
     lcd.setCursor(0,0);
  lcd.print("Getting GPS Lock");
  delay(2000);// GPS info
    
  //If no input from GPS is detected, it could mean the GPS is not connected or wired correctly
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
   // while(true);
  }
}

//Display the GPS info coming in




//GET INFO FROM GPS MODULE
void getGPSInfo()
{
  //Do the following if the incoming GPS data is VALID
    Serial.print(F("Location: ")); 
  for(int a=0; a<4; a++){
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
     lcd.clear();
   lcd.setCursor(0,0);
  lcd.print("GPS Lock");
     lcd.setCursor(0,1);
  lcd.print("   Successful    ");
  delay(2000);
}


  //Do the following if the incoming GPS data is INVALID
  else
  {
    Serial.print(F("INVALID"));
     lcd.clear();
    lcd.setCursor(0,0);
  lcd.print("GPS Lock");
     lcd.setCursor(0,1);
  lcd.print("Not Successful");
  delay(2000);
  }


//Get the date from the GPS Module
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }


//Get the time from the GPS Module
  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

}



//GET THE LATITUDE
float getLatitude(){

  if (gpsModule.available() > 0){
    if (gps.encode(gpsModule.read()))
    
   if(gps.location.isValid()){
   for(int x=0; x<5; x++)
  {
    longitude = gps.location.lat();
  }
   return(latitude);
 }
  }
  else{  
    for(int x=0; x<5; x++)
  {
    longitude = gps.location.lat();
  }
    return(latitude);
 
  }
}


//GET THE LONGITUDE
float getLongitude(){
  if (gpsModule.available() > 0){
    if (gps.encode(gpsModule.read()))
   if(gps.location.isValid()){
   for(int x=0; x<5; x++)
  {
    longitude = gps.location.lng();
    Serial.print("Longitude:");  Serial.println(longitude);
  }
   return(longitude);
 }
  }
  else{  
    for(int x=0; x<5; x++)
  {
    longitude = gps.location.lng();
  }
    return(longitude);
 
  }
}



/*---------------------------------------------------
  GSM MODULE FUNCTIONS
----------------------------------------------------*/


//The Function Which Sends Text Messages. Takes in 3 parameters..
// 1. First parameter specifies what kind of message it should send
//2. Second parameter is the Fuel level measured
//3. Third parameter is a pointer(reference to longitude and latitude)

void sendSMS(int a, float b, float c, float d){
   //sets the PIN code

 
    sendATcommand("AT+CPIN=****", "OK", 2000);
    
    delay(3000);
    
    Serial.println("Connecting to the network...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println("GSM Connecting..");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.print("Setting SMS mode...");
    lcd.setCursor(0, 1);
    lcd.print("Setting SMS mode");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
     lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sending Info");

    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    { 


         lcd.clear();
         if(a == 1){
         //Serial.println("Significant Level Change Detected");
        //Serial.print("New Level is "); Serial.print(b); Serial.print(" ml");   
                Serial.print("RLFM; "); Serial.print("100; "); Serial.print(b); Serial.print("; "); Serial.print("6.670980; "); Serial.print( "-1.561330; "); 
        
         }
         else if(a == 2){
         //  Serial.println("Reply to Query");
       // Serial.print("New Level is "); Serial.print(b); Serial.print(" ml"); 
                Serial.print("RLFM; "); Serial.print("200; "); Serial.print(b); Serial.print("; "); Serial.print("6.670980; "); Serial.print( "-1.561330; "); 
                }

         else if(a == 3){
          // Serial.println("Status Update:");
        //Serial.print("Current Level is "); Serial.print(b); Serial.print(" ml"); 
            Serial.print("RLFM; "); Serial.print("300; "); Serial.print(b); Serial.print("; "); Serial.print("6.670980; "); Serial.print( "-1.561330; "); 
            }
        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
         lcd.clear();
            Serial.print("Sent ");  
             lcd.setCursor(0,0);
            lcd.print("Level & Position");
           lcd.setCursor(0,1);
            lcd.print("Sent");
             delay(2000); 
        }
        else
        {
            Serial.print("error ");
            lcd.setCursor(0,0);
            lcd.print("Error Sending ");
           lcd.setCursor(0,1);
            lcd.print("Level & Position");
             delay(2000);
        }
    }
    else
    {
        Serial.print("error ");
        lcd.clear();
          lcd.setCursor(0,0);
            lcd.print("Error Setting ");
           lcd.setCursor(0,1);
            lcd.print("SMS Mode");
             delay(2000);

             lcd.clear();
          lcd.setCursor(0,0);
            lcd.print("Retrying to ");
           lcd.setCursor(0,1);
            lcd.print("Set SMS Mode");
             delay(2000);
        Serial.println(answer, DEC);
    }

}


//Function to turn on the GSM module
void power_on(){

    uint8_t answer=0;
    
    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Setting up GSM..");
        
    
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
                     lcd.setCursor(0,1);
                      lcd.print("Success!");
            }
        }
    // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}


//SETTING UP GSM RECEIVE TEXT MODE
void setupGSMReceiveText(){
Serial.begin(57600);
Serial.println("Setting Up GSM Receive Text Mode");
Serial.println("AT\r");
delay(1000);
Serial.println("AT+CMGF=1\r");
delay(1000);
Serial.println("AT+CNMI=2,2,0,0,0\r");
}



