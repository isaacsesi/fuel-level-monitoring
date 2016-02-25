 /* 
 Remote Fuel Level Monitoring System (Proteus Simulation)
 AUTHORS: ISAAC SESI
 CONTRIBUTORS: ABDUL-LATIF AMADU, MICHAEL AGYEI-KENA
 */

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <NewPing.h>
#include <LiquidCrystal.h>



#define GPS_TX 9
#define GPS_RX 8
#define GPSBAUD 4800

 
const int TRIGGER_PIN = 7;
const int ECHO_PIN  =  6;

const int MAX_DISTANCE = 200;
const int SAMPLE_INTERVAL = 25;
const int PING_ITERATIONS = 3;
const int UPDATE_INTERVAL = 1500;

int previousMillis = 0;
int incomingByte = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
TinyGPS gps;
SoftwareSerial nss(GPS_RX , GPS_TX);
 
void setup() {
   Serial.begin(57600);
   nss.begin(GPSBAUD);
  lcd.begin(20, 4);
  Serial.println("");
  Serial.println("Initializing GPS");
  lcd.setCursor(0,0);
  lcd.print("Initializing GPS...");
  delay(100);
  Serial.println("...waiting for lock...");
   lcd.setCursor(1,1);
  lcd.print("waiting for lock...");
  delay(100);
  Serial.println("");
  Serial.print("SUCCESS!");
  lcd.setCursor(1,6);
  lcd.print("SUCCESS!");
  delay(100);
  lcd.clear();
  getFuelLevel();
   getGPSLocation();

}
 
void loop() {
checkAlertInterval();
}


void checkAlertInterval()
{
  
  unsigned long currentMillis = millis();
 
    if(currentMillis - previousMillis > UPDATE_INTERVAL) {
      previousMillis = currentMillis;   
      getFuelLevel();
      getGPSLocation();
      return;
  }

    else if(Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();

                if(incomingByte == 1){
                getFuelLevel();
                getGPSLocation(); 
                return; 
              }
              else{
                Serial.println("Please enter a valid query");
                return;
              }
        }

    else
      return;
}


void getFuelLevel(){
  int t = sonar.ping_median(PING_ITERATIONS);                 // microseconds
  int d = sonar.convert_cm(t);          // centimeters
  Serial.println("");
  Serial.print("Fuel level is "); Serial.print(sonar.convert_cm(t)); Serial.print("cm");
  Serial.println("");
  lcd.setCursor(1,0);
  lcd.println("Fuel level is "); lcd.print(d); lcd.print("cm");
}


void getGPSLocation(){

   while (nss.available())
  {
    int GPSdata = nss.read();
    if (gps.encode(GPSdata))
    {
  
            int year;
            byte month, day, hour, minute, second, hundredths;
            float lat, lon;
             
             gps.f_get_position(&lat, &lon); // returns +/- latitude/longitude in degrees
            float speed = gps.f_speed_kmph(); // speed in km/hr
           gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
         

              Serial.print("Lat/Long: "); 
              Serial.print(lat,5); 
              Serial.print(", "); 
              Serial.println(lon,5);
              Serial.print("Speed(kmph): "); Serial.println(gps.f_speed_kmph());
              Serial.println();
              Serial.print("Date: "); Serial.print(month, DEC); Serial.print("/"); 
              Serial.print(day, DEC); Serial.print("/"); Serial.print(year);
              Serial.print("  Time: "); Serial.print(hour, DEC); Serial.print(":"); 
              Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
              Serial.print("."); Serial.println(hundredths, DEC);
    }
  }
}
