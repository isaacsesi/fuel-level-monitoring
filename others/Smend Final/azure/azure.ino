//INCLUDE REQUIRED LIBRARIES
#include <LiquidCrystal.h>
#include <Wire.h>   
#include <EEPROM.h>
#include <Time.h> 
#include <DS1307RTC.h>
#include <String.h>
#include <TimeAlarms.h>
#include <GSM.h>
void digitalClockDisplay();
void printDigits(int digits);



//GLOBAL VARIABLE DECLARATIONS AND INTITIALIZAATIONS
//gsm
int8_t answer;
int onModulePin= 2;
char aux_string[30];
char phone_number[]="0277080225";

int addr = 0;
int blinkCount = 0;   // 0-255
int threshold = 400;
char str[5] = "kWh";
String info1 ="Your Total Energy Consumption:\nEnergy Spent:";// I want
//to pass the consumption and amount due as a string to GSM
String info2 = "kWh\nAmount Due: GHS"; 

String  currency = "Amount: GHS ";
String kWh_reading = "Energy: ";
String SMS;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int relayPin = 8;
char val1;
 int kilowatt=EEPROM.read(addr);
  double amount = kilowatt*0.076;
 
//SETUP FUNCTION 
void setup() {
   setTime(8,29,0,4,5,15); // set time to Monday 8:29:00am May 4 2015
   
   lcd.begin(16, 2);  //initialize the 20*4 LCD
   lcd.setCursor(0,0);// set cursor to col 1 row 1 
   
  // Configure I/O pins as input/output:

  pinMode(A2, INPUT);  // LDR sensor
  pinMode(relayPin, OUTPUT);
 
  // Start serial communication with a PC:
  Serial.begin(9600);
  
  //setup gsm module
     Serial.println("Starting...");
    power_on();
    delay(3000);
    
    //sets the PIN code
    sendATcommand("AT+CPIN=****", "OK", 2000);
    
    delay(3000);
    
    Serial.println("Connecting to the network...");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );
            Alarm.timerRepeat(15, Repeats);            // timer for every 5 minutes 

}


//LOOP FUNCTION
void loop() {
 // Read LDR sensor (0-1023 = 0-5V):
 digitalClockDisplay();
  int val = (analogRead(A2)) ;
   //Serial.print("Sensor Value is: ");
   //Serial.print(val);
   //Serial.println("   ");
  // Notify relevant change with green LED:
  
  //CHECK IF LED FROM ENERGY METER HAS BLINKED
  if ( val < threshold){
    blinkCount++;
    
    
    //10 BLINKS OF THE LED EQUALS ONE KILLOWATT HOUR       
  if (blinkCount >=10 &&blinkCount% 10 == 0 )
      {
          //blinkCount = 0;
          kilowatt++;
          amount+= 0.076; 
         // Serial.print("Sensor Value is: ");
         // Serial.print(val);
         // Serial.println(blinkCount);
         // Serial.print("   ");
         // Serial.println("");
          lcd.setCursor(0,0);
          //Serial.println(kilowatt);
          lcd.print (kWh_reading + kilowatt + " kWh");
          Serial.print(amount);
          lcd.setCursor(0,1);
          lcd.print(currency);
         lcd.print(amount);   
        
        
         
 //CODE TO STORE KILOWATT VALUE IN EEPROM
         
         int i;
         if (i % 100 == 0)
         {
           EEPROM.write(addr, kilowatt);
        
         }
         i++;     

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
  /*
  Serial.print("Adress is:  ");
  Serial.print(addr);
  Serial.print("\t");
  Serial.print("Value is:  ");
  Serial.print(value, DEC);
  Serial.println();
  */
  if (addr == 512)
    addr= 0;
   
  
  
  /*CODE TO CHECK WHETHER ITS TIME TO SEND A TEXT MESSAGE
   time_t t = now();
   int hour2 = hour();
   int minute2 = minute();
   int hour1=hour(t);
   int minute1=minute(t);
   if ((hour2 -  hour1 == 0 && minute2 -  minute1 >= 1)||(hour2-hour1== 1 && (minute2+60)-minute1 >=5))
  {
    Serial.print ("Hello, your energy consumption for this month is: ");
    Serial.print(kilowatt);
    Serial.print("kW");
    Serial.println("And your bill for this month is ");
   Serial.print("GHS ");
 Serial.print(amount);
  
  }*/

//CODE TO TRIGGER RELAY
/*
 if( Serial.available() )       // if data is available to read
{
 {
   val1 = Serial.read();         // read it and store it in 'val'
 
 }
 if( val1 == 'A' ) 
 {
     Serial.println("Switching off supply...: ");
    // set the LED with the ledState of the variable:
    digitalWrite(relayPin, HIGH);
}

else if(val1 == 'B'){
    Serial.println("Switching on supply...");                                                                                                                                                                                                                                                                                  
    // set the LED with the ledState of the variable:
    digitalWrite(relayPin, LOW);
}


}
*/
delay(100);
 sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    {
        Serial.println("Hello, your consumption for this month is");
        Serial.print(kilowatt);
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


//Time alarm (calls reading after 5 mins)
void Repeats(){
  Serial.println(" 5 minute timer"); 
  Serial.print("Setting SMS mode...");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    {
        Serial.println("Hello, your consumption for this month is");
        Serial.print(kilowatt);
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



//gsm functions
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

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println(); 
}

void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
