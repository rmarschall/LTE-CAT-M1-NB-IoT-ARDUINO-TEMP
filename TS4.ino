#include "Adafruit_FONA.h"
#include <SPI.h>
#include <LiquidCrystal.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DFRobot_RGBLCD1602.h"

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

//Set temperature update frequency(in minutes)
const int interval_time = 1;

DFRobot_RGBLCD1602 lcd(/*lcdCols*/16,/*lcdRows*/2);  //16 characters and 2 lines of show

#define SIMCOM_7000
#define FONA_PWRKEY 6
#define FONA_RST 7
#define FONA_TX 10
#define FONA_RX 11
#define MODEM_RST 5

#define ONE_WIRE_BUS 0



OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();



//FONA hardware reset function
void modem_reset() {
  Serial.println("\nModem hardware reset\n");
  pinMode(MODEM_RST, OUTPUT);
  digitalWrite(MODEM_RST, LOW);
  delay(260); //Treset 252ms
  digitalWrite(MODEM_RST, HIGH);
  delay(10000); //Modem takes longer to get ready and reply after this kind of reset vs power on
}

//FONA Signal strength display function
void signalStrength() {
  uint8_t n = fona.getRSSI();
  int8_t r;

  lcd.print(F("RSSI = ")); 
  lcd.print(n); 
  lcd.print(": ");
  if (n == 0) r = -115;
  if (n == 1) r = -111;
  if (n == 31) r = -52;
  if ((n >= 2) && (n <= 30)) {
     r = map(n, 2, 30, -110, -54);
  }
  lcd.print(r); 
  lcd.println(F(" dBm"));
  delay(5000);

  lcd.clear();
}


void setup() {
  
  sensors.begin();
  lcd.init();

  //Set initial LCD color
  lcd.setRGB(colorR, colorG, colorB);
  pinMode(7, HIGH);

  lcd.print("LULU-COM BOOTING...");

  delay(5000);

  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH); // Default state

  pinMode(FONA_PWRKEY, OUTPUT);

  // Turn on the module by pulsing PWRKEY low for a little bit
  // This amount of time depends on the specific module that's used
  fona.powerOn(FONA_PWRKEY); // Power on the module
  lcd.clear();
  Serial.begin(9600);
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take several seconds)"));
  lcd.print("Initializing....");

  // Software serial:
  fonaSS.begin(115200); // Default SIM7000 shield baud rate

  Serial.println(F("Configuring to 9600 baud"));
  
  fonaSS.println("AT+IPR=9600"); // Set baud rate
  delay(100); // Short pause to let the command run
  fonaSS.begin(9600);
  

  //Fona board check 
  if (! fona.begin(fonaSS)) {
    Serial.println(F("Couldn't find FONA"));
    lcd.print("Couldn't find FONA");
    digitalWrite(FONA_PWRKEY, HIGH);
    modem_reset();
  }
  else{
    lcd.print("HW found!");
    delay(5000);
  }

  lcd.clear();

  // AT+CFUN=1
  fona.setFunctionality(1);

  fona.setNetworkSettings(F("hologram"));
  delay(10000);
  Serial.println(F("startup complete"));

  lcd.print("ENABLING RADIO");
  delay(2000);
  Serial.println(F("TURNING ON RADIO"));
  
  lcd.clear();

  //Fona enable data
  if (!fona.enableGPRS(true)){
    Serial.println(F("Failed to turn on"));
    lcd.print("FAILED TO CONNECT!");
    delay(5000);
  }
  else{
    lcd.print("CONNECTED!");
    delay(2000);
  }

  lcd.clear();
}



void loop() {
  
  //Data publish frequncy(in minutes)
  
  //
  sensors.requestTemperatures();
  
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

  //LCD backlight color statement 
  if(temperatureF <= 70){
      lcd.setRGB(0, 0, 255);  
  }
  else if (temperatureF <= 78 && temperatureF >= 71 ){
      lcd.setRGB(0, 255, 0); 
  }
  else if(temperatureF >= 79){
      lcd.setRGB(255, 0, 0); 
  }

  char result[8];
  dtostrf(temperatureF, 6, 2, result);

  lcd.print(temperatureF);
  delay(5000);
  lcd.clear();
  Serial.print(temperatureF); Serial.println("*F");
  
  signalStrength();
  
  if (! fona.begin(fonaSS)) {
    Serial.println(F("Couldn't find FONA"));
    //modem_reset();
    setup();
  }

  lcd.clear();
  lcd.print("Temp:");
  lcd.print(temperatureF);
  delay(5000);
  
  lcd.clear();

  if(fona.enableGPRS(true) == false){
    fona.enableGPRS(true);
  }

   delay(2000);
   uint16_t statuscode;
   int16_t length;
   char url[80];

   Serial.println(result);

   //Replace with thingspeak api key
   sprintf(url,"api.thingspeak.com/update?api_key=XXXXXXXXXXXXXXX&field1=%s", result);

   Serial.println(url);
   
   fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length);
    lcd.print("Update pushed!");
    lcd.setCursor(0,1);
    lcd.print("via LTE@");
    lcd.print(interval_time);
    lcd.print("m int");
 
   fona.HTTP_GET_end();
   
   fona.enableGPRS(false); 
  delay(60000*interval_time);

}
