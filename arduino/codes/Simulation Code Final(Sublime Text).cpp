#include <TinyGPS.h>
#include <LiquidCrystal.h>

const int trigPin = 7;
const int echoPin = 6;
long duration, cm;
const int MAX_DISTANCE = 100;
const int MIN_DISTANCE = 0;
const long INTERVAL = 5000;
const int FUEL_DIFFERENCE = 50;

 float flat, flon;
  unsigned long age;

int previousLevel = 0;
long previousMillis = 0;


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
TinyGPS gps;

void setup() {
  Serial.begin(9600);
  lcd.begin(20,4);
  lcd.setCursor(0,0);
  lcd.print("Initializing GSM...");
  delay(100);
  lcd.setCursor(1,6);
  lcd.print("SUCCESS");
  delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
   lcd.print("Initializing GPS...");
  delay(100);
  lcd.setCursor(1,6);
  lcd.print("SUCCESS");
  Serial.println("SYSTEM STATUS: GSM OK, GPS OK");
  Serial.println();
  previousLevel = getFuelLevel(); 
  Serial.print("Initial Fuel level is: "); Serial.println(previousLevel);
  Serial.println("-----------------------------------");
  Serial.println("");
  lcd.clear();
  lcd.print("Current Level:"); lcd.print(previousLevel);lcd.print(" ltr");
}


void loop(){
  checkFuelLevelChange();
  checkForSystemQuery();
  checkUpdateIntervalTimeout();
}



void checkFuelLevelChange(){
  int currentLevel = getFuelLevel();
     if(abs(currentLevel - previousLevel >= FUEL_DIFFERENCE)){
      sendSMSAlert(1);
      return;
    } else
        return;
  
}

void checkForSystemQuery(){
  bool newQuery = false;
  while(Serial.available()){
    char userInput = Serial.read();
    if(userInput=='A' || userInput=='a')
      newQuery = true;
  }
  if(newQuery)
     sendSMSAlert(2);
     return;
}

void checkUpdateIntervalTimeout(){
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > INTERVAL) {
    previousMillis = currentMillis; 
    sendSMSAlert(3);
  }
    return;
}

void sendSMSAlert(int a){
    int currentFuelLevel = getFuelLevel();
    getGPSData();

  if(a == 1){
    Serial.println("Significant Change Detected:");
    Serial.println("-----------------------------");
    Serial.print("Current Fuel level is ");Serial.print(currentFuelLevel); Serial.println("litres");
    Serial.print("Tanker is currently at ");Serial.print(flat); Serial.print(",");Serial.println(flon);
    Serial.println("");
    
  } else if(a == 2){
    Serial.println("Response to System Query:");
    Serial.println("-------------------------");
    Serial.print("Current Fuel level is ");Serial.print(currentFuelLevel); Serial.println("litres");
    Serial.print("Tanker is currently at ");Serial.print(flat); Serial.print(",");Serial.println(flon);
    Serial.println("");
  }

  else if(a == 3){
    Serial.println("System Status:");
    Serial.println("-----------------");
    Serial.print("Current Fuel level is ");Serial.print(currentFuelLevel); Serial.println("litres");
    Serial.print("Tanker is currently at ");Serial.print(flat); Serial.print(",");Serial.println(flon);
    Serial.println("");
    lcd.clear();
    lcd.print("Current Level:"); lcd.print(currentFuelLevel);lcd.print(" ltr");
  }
 
}



int getFuelLevel(){
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  int level = duration /29 /2;
  
  if(level >= MIN_DISTANCE && level <= MAX_DISTANCE){
    return level;
  } else {
    return -1;
  } 
}




void getGPSData(){
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
     // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    gps.f_get_position(&flat, &flon, &age);
    /*
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    */
  }
  
  gps.stats(&chars, &sentences, &failed);
  /*
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  */
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
}