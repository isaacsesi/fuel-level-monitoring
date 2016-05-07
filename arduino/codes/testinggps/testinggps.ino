#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define TX 8
#define RX 9
#define GPSBAUD 9600
//SoftwareSerial terminal(RX, TX);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
char incomingData;

void setup(){
  Serial.begin(9600);
  lcd.begin(20,4);
 // gps.begin(GPSBAUD);
}

void loop(){
  char gpsData[]; 
  if (Serial.available()){
  for (int i = 0; i<300; ++i){
    gpsData[i]=Serial.read();
  }
   for (int i = 0; i<300; ++i){
     Serial.print(gpsData[i]);
  }
 }
}



