//INCLUDE REQUIRED LIBRARIES
#include <LiquidCrystal.h>
#include <Wire.h>   
#include <EEPROM.h>
#include <Time.h> 
#include <DS1307RTC.h>
#include <String.h>
#include <String.h>

//GLOBAL VARIABLE DECLARATIONS AND INTITIALIZAATIONS
 int addr = 0;
byte blinkCount = 0;   // 0-255
int threshold = 300;
boolean ledON = false; // ON=true, OFF=false
int kilowatt=0;
char str[5] = "kWh";
String info1 ="Your Total Energy Consumption:\nEnergy Spent:";// I want
//to pass the consumption and amount due as a string to GSM
String info2 = "kWh\nAmount Due: GHS"; 
double amount = 0;  //amount in cedis
String  currency = "Amount: GHS ";
String kWh_reading = "Energy: ";
String SMS;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int greenLed = 9;
int redLed = 10;
int relayPin = 8;
int relayState = LOW;
char val1;

 
 
//SETUP FUNCTION 
void setup() {
  
   lcd.begin(16, 2);  //initialize the 20*4 LCD
   lcd.setCursor(0,0);// set cursor to col 1 row 1 
   
  // Configure I/O pins as input/output:
  pinMode(greenLed, OUTPUT); // Green LED
pinMode(redLed, OUTPUT); // Red LED
  pinMode(A2, INPUT);  // LDR sensor
 
  // Set default states for output pins:
  digitalWrite(greenLed, LOW);  // Green LED off
  digitalWrite(redLed, HIGH); // Red LED on
 
  // Start serial communication with a PC:
  Serial.begin(9600);
}


//LOOP FUNCTION
void loop() {
 // Read LDR sensor (0-1023 = 0-5V):
  int val = analogRead(A2);
   Serial.print("Sensor Value is: ");
              Serial.print(val);
                      Serial.println("   ");
  // Notify relevant change with green LED:
  
  //CHECK IF LED FROM ENERGY METER HAS BLINKED
  if ( val <= threshold){
    blinkCount++;
    
    
    //10 BLINKS OF THE LED EQUALS ONE KILLOWATT HOUR       
  if (blinkCount% 10 == 0)
      {
          //blinkCount = 0;
          kilowatt++;
          amount+= 0.0976; 
          Serial.print("Sensor Value is: ");
          Serial.print(blinkCount);
          Serial.print("   ");
          Serial.println("");
          lcd.setCursor(0,0);
          //Serial.println(kilowatt);
          lcd.print (kWh_reading + kilowatt + " kWh");
          Serial.print(amount);
          lcd.setCursor(0,1);
          lcd.print(currency);
         lcd.print(amount);   
        
        
         
 //CODE TO STORE KILOWATT VALUE IN EEPROM
         /*
         int i;
         if (i % 100 == 0)
         {
           EEPROM.write(addr, kilowatt);
        
         }
         i++;     
*/
      }
      
 }
     else
      {
        lcd.setCursor(0,0);
          lcd.print (kWh_reading + kilowatt + " kWh");
          lcd.setCursor(0,1);
          lcd.print(currency);
         lcd.print(amount );
      }
 

//READM FROM THE EEPROM IN THE EVENT OF POWER OUT
  int value = EEPROM.read(addr);
  
  Serial.print("Adress is:  ");
  Serial.print(addr);
  Serial.print("\t");
  Serial.print("Value is:  ");
  Serial.print(value, DEC);
  Serial.println();
  if (addr == 512)
    addr= 0;
    
  delay(500);

  // Wait 5 ms before next read (200 Hz sampling rate):
  delay(100);
  
  
  
  //CODE TO CHECK WHETHER ITS TIME TO SEND A TEXT MESSAGE
   time_t t = now();
   int hour2 = hour();
   int minute2 = minute();
   int hour1=hour(t);
   int minute1=minute(t);
   if ((hour2 -  hour1 == 0 && minute2 -  minute1 >= 5)||(hour2-hour1== 1 && (minute2+60)-minute1 >=5))
  {
    Serial.print ("Hello, your energy consumption for this month is: ");
    Serial.print(kilowatt);
    Serial.print("kilowatt");
    Serial.println("And your bill for this month is");
   Serial.print(currency);
  Serial.print(amount);
  
  }

//CODE TO TRIGGER RELAY
if( Serial.available() )       // if data is available to read
 {
   val1 = Serial.read();         // read it and store it in 'val'
  
 
 }
 if( val1 == 'S' ) 
 {
   
     Serial.print("Switch supply state requested: ");
   Serial.println(val);
    if (relayState == LOW)
      relayState = HIGH;
    else
      relayState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(relayPin, relayState);

}
}
