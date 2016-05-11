
/* 
AUTOMATIC FARM IRRIGATION SYSTEM

Code Written by Isaac Sesi
www.currentcity.cc
19th April 2016
Licence: Creative Commons - Non Commercial Share-Alike with Attribution

How The Program Works
1. The System measures the level of water in the social and displays 
the status (LOW, OK OR HIGH) 
2. When the level measured is LOW, the system sends an SMS to the authorized phone
and turns on the pump (represented by a motor)
3. If the level measured comes back to either OK or high, the pump turns off
4. At anytime the user can send a text message to request for the water level or
turn on the pump.



*/

#include <LiquidCrystal.h>



LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
unsigned int waterLevel = 0;
unsigned int waterLevelSensor = A0;

#define motor 7

void setup() {
  lcd.begin(16, 2);
  Serial.begin(115200);
   pinMode(onModulePin, OUTPUT);
  pinMode(waterLevelSensor, INPUT);
  pinMode(motor, OUTPUT);
     power_on();
    delay(2000);
  
}

void loop(){
waterLevel = getWaterLevel();
Serial.println(waterLevel);

//Display a LOW status on LCD, Send SMS to user and turn on motor
if(waterLevel > 0 && waterLevel <= 600){
  digitalWrite(motor, HIGH);
  display(1);
  //thinking of a way to keep the f*cking function from being called too many timesv 321
  sendSMS(1);

}

//Display an OK status on LCD, turn of motor(if on)
else if(waterLevel > 600 && waterLevel <= 900){
    display(2);
    digitalWrite(motor, LOW);
}
//Display a High status on LCD, turn of motor(if on)
else if(waterLevel > 900 && waterLevel <= 1023){
    display(3);
    digitalWrite(motor, LOW);
}

//Check if user has sent an SMS
 while (Serial.available())
    {
      char messageReceived = Serial.read();

      //If user texts '1' to request for the water level in soil, send an SMS with the requested info
      if(messageReceived == '1'){
        sendSMS(3);
      }

       //If user texts '2' to turn on motor, perform requested option and send a feedback to user
      else if(messageReceived == '2'){
        digitalWrite(motor, HIGH);
        sendSMS(4);
      }
}
 
}

int getWaterLevel(){
  waterLevel = analogRead(waterLevelSensor);
  return waterLevel;
}


void display(int a){
  lcd.setCursor(0, 0);
  if(a == 1){
    lcd.print("WATER LEVEL: LOW");
    lcd.setCursor(0,1);
    lcd.print("MOTOR STATUS: ON ");
  }
   if(a == 2){
    lcd.print("WATER LEVEL: OK ");
    lcd.setCursor(0,1);
    lcd.print("MOTOR STATUS:OFF");
  }
   if(a == 3){
    lcd.print("WATER LEVEL:HIGH");
    lcd.setCursor(0,1);
    lcd.print("MOTOR STATUS:OFF");
  }
}





