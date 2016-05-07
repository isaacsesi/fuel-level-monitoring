#include <LiquidCrystal.h>
#include <Wire.h>   

// a basic DS1307 library - http://www.arduino.cc/playground/Code/Time
#include <String.h>

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

void loop() {
 // Read LDR sensor (0-1023 = 0-5V):
  int val = analogRead(A2);
   Serial.print("Sensor Value is: ");
              Serial.print(val);
                      Serial.println("   ");
  // Notify relevant change with green LED:
  if ( val <= threshold){
    blinkCount++;
           
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
          lcd.print(currency + amount );
      }
      
  
 }
     else
      {
        lcd.setCursor(0,0);
          lcd.print (kWh_reading + kilowatt + " kWh");
          lcd.setCursor(0,1);
          lcd.print(currency + amount );
      }
 
 

  // Wait 5 ms before next read (200 Hz sampling rate):
  delay(100);

}
