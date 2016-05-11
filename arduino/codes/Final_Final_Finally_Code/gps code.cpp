
#include <LiquidCrystal.h>
#include  <EEPROM.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

//Ultrasonic Sensor
const int trigPin = 7;
const int echoPin = 6;


//GSM Module
int8_t answer;
int onModulePin= 10;
char aux_string[30];
char phone_number[]="0501359999";

//GPS Module
static const int RXPin = 9, TXPin = 8;
static const uint32_t GPSBaud = 9600;
float latitude;
float longtide;


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
void setupGPS();
void displayInfo();

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

   //SETUP GPS MODULE
   setupGPS();
   delay(1000);
  
//previousLevel = EEPROM.read(addr); 
// newLevel = getFuelLevel();


 Serial.println(previousLevel);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Last: ");
  lcd.print(previousLevel); lcd.print("ml   ");
  lcd.setCursor(0,1);
  lcd.print("New: ");
  lcd.print(newLevel); lcd.print("ml    ");

  if(abs(newLevel - previousLevel >= FUEL_DIFFERENCE)){
   // sendSMS(1, newLevel);
  }
}

void loop(){
}

void setupGPS(){
  // This sketch displays information every time a new sentence is correctly encoded.
  lcd.setCursor(0,0);
  lcd.print("Setting up GPS..");
  delay(3000);

  while (gpsModule.available() > 0)
    if (gps.encode(gpsModule.read()))
    lcd.clear();
     lcd.setCursor(0,0);
  lcd.print("Getting GPS Lock");
  delay(2000);
displayInfo();
    
  
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}


void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    int gpsLock = 1;
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




//GSM FUNCTIONS

void sendSMS(int a, float b){
   //sets the PIN code

  // float *gpsInfo;
 //  gpsInfo
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

