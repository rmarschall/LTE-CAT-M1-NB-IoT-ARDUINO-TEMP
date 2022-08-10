#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include <Wire.h>
//#include "SFE_ISL29125.h"
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdlib.h>

//SFE_ISL29125 RGB_sensor;

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define FONA_KEY 7
#define FONA_PS 8
#define led 13
#define trigger 6

SoftwareSerial myfona = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(&myfona, FONA_RST);

int i = 1; //loop counter
char channelid[]="XXXXXXXX"; //put the Thingspeak Channel-ID here


void setup() {
  //Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(trigger, INPUT);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
     if (RGB_sensor.init())
  {
    for(int i=0;i<2;i++){
      unsigned int trash1 = RGB_sensor.readRed();
      unsigned int trash2 = RGB_sensor.readGreen();
      unsigned int trash3 = RGB_sensor.readBlue();
      delay(1000);
      }
  }
  fona.setGPRSNetworkSettings(F("APN"), F("User"), F("Password")); //put your APN settings here
  pinMode(FONA_KEY, OUTPUT);
  TurnOffFona();
  delay(1000);
 }


void loop() {
  digitalWrite(led, HIGH);
  pinMode(trigger, INPUT);
  if(i % 3 == 0)
    { 
      SendGPS();
      delay(1000); 
    }
    else
    {
     SendGPS();
     delay(1000);
     //Send2TP();
    }
  
  delay(100);
  TurnOffFona();
  delay(1000);
  pinMode(trigger, OUTPUT);
  digitalWrite(led, LOW);
  i++;
  
sleepabit(3350);
}



void SendGPS()
{
  //prepare sensor data
  unsigned int red1 = RGB_sensor.readRed();
  unsigned int green1 = RGB_sensor.readGreen();
  unsigned int blue1 = RGB_sensor.readBlue();
  unsigned int red = sqrt(sqrt(red1*red1/1));
  unsigned int green = sqrt(sqrt(green1*green1/1));
  unsigned int blue = sqrt(sqrt(blue1*blue1/1));
  char value_red[3+1];
  char value_green[3+1];
  char value_blue[3+1];
  sprintf(value_red,"%d",red);
  sprintf(value_green,"%d",green);
  sprintf(value_blue,"%d",blue);
  
  char replybuffer[80];
  uint16_t returncode;
  TurnOnFona();
  delay(1000);
  fona.begin(4800);
  delay(3500);
  GetConnected();  
  delay(3000);
  fona.enableGPRS(true);
  delay(3000);
  
  //get battery status
  uint16_t vbat;
  fona.getBattPercent(&vbat);
  char value_bat[20];
  sprintf(value_bat,"%d",vbat);
  
  //get number of loops
  char loops[10];
  sprintf(loops,"%d",i);
  
  //try to get GPS info     
  if (!fona.getGSMLoc(&returncode, replybuffer, 250))
         Serial.println(F("GPS Lookup Failed!")); 
       if (returncode == 0) {
         //parse lat and lon
           char *lat;
           char *lon;
           char delimiter[] = ",";
           char *ptr;
           ptr = strtok(replybuffer, delimiter);
           int h = 0;
           while(ptr != NULL) {
             if(h==0)
              lat=ptr;
             if(h==1)
              lon=ptr;
             ptr = strtok(NULL, delimiter);
             h++;
            } //while ptr end
            
            //send the stuff to TP
            char url[300];
            uint16_t statuscode;
            int16_t length;
            sprintf(url,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s","http://[your thingspeak installation]/update?key=",channelid,"&field1=",value_red,"&field2=",value_green,"&field3=",value_blue,"&field4=",value_bat,"&field5=",loops,"&lat=",lat,"&long=",lon,"token=tbd"); //put your tp installation here
            flushSerial();
            if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length)) 
              {
              Serial.println(F("Get Failed!"));
              } 
           fona.HTTP_GET_end();  
       } //returncode 0 end 
       else 
       {
         //send the stuff to TP without GPS
            char url[200];
            uint16_t statuscode;
            int16_t length;
            sprintf(url,"%s%s%s%s%s%s%s%s%s%s%s%s%s","http://[your thingspeak installation]/update?key=",channelid,"&field1=",value_red,"&field2=",value_green,"&field3=",value_blue,"&field4=",value_bat,"&field5=",loops,"token=tbd"); //put your tp installation here
            flushSerial();
            if (!fona.HTTP_GET_start(url, &statuscode, (uint16_t *)&length)) 
              {
              Serial.println(F("Get Failed!"));
              } 
           fona.HTTP_GET_end();  
       
       }
   delay(100);
   GetDisconnected();
   delay(1000);
  TurnOffFona();
}





boolean SendATCommand(char Command[], char Value1, char Value2) {
  unsigned char buffer[64];                                  
  unsigned long TimeOut = 20000;    
  int count = 0;
  int complete = 0;
  unsigned long commandClock = millis();                      
  fona.println(Command);
  while (!complete && commandClock <= millis() + TimeOut)      
  {
    while (!fona.available() && commandClock <= millis() + TimeOut);
    while (fona.available()) {                                
      buffer[count++] = fona.read();                          
      if (count == 64) break;
    }
    //Serial.write(buffer, count);                          
    for (int i = 0; i <= count; i++) {
      if (buffer[i] == Value1 && buffer[i + 1] == Value2) complete = 1;
    }
  }
  if (complete == 1) return 1;                             
  else return 0;
}



void GetConnected()
{
  long elapsedTime ; 
  uint8_t n = 0;
  long startTime ; 
  startTime = millis(); //start time of the try
  do
  {
    n = fona.getNetworkStatus();  // Read the Network / Cellular Status
    //Serial.print(F("Network status "));
    //Serial.print(n);
    //Serial.print(F(": "));
    if (n == 0);// Serial.println(F("Not registered"));
    if (n == 1);// Serial.println(F("Registered (home)"));
    if (n == 2);// Serial.println(F("Not registered (searching)"));
    if (n == 3);// Serial.println(F("Denied"));
    if (n == 4);// Serial.println(F("Unknown"));
    if (n == 5);// Serial.println(F("Registered roaming"));
    elapsedTime=millis() - startTime; 
    if(elapsedTime > 80000)
      {
      delay(1000);
      TurnOffFona();
      pinMode(trigger, OUTPUT);
      digitalWrite(led, LOW);
      sleepabit(1800); //if we don't get on the network we will sleep a bit
      startTime = millis(); //reset start-time  
      digitalWrite(led, HIGH);
      pinMode(trigger, INPUT);
      TurnOnFona();
      delay(100);
      fona.begin(9600);    
      //break;
      }
    ////wdt_reset();
  } 
  while (n != 5); //be careful - this is code expects a SIM that registeres as roaming - change this to "1" if your SIM is registering normally
  //if (n !=5)
      //sleepabit(3600);
  ////wdt_reset();
}

void GetDisconnected()
{
  fona.enableGPRS(false);
  //Serial.println(F("GPRS Serivces Stopped"));
}

void TurnOnFona()
{
  //Serial.println("Turning on Fona: ");
  while(digitalRead(FONA_PS)==LOW)
    {
    digitalWrite(FONA_KEY, LOW);
    }
    digitalWrite(FONA_KEY, HIGH);
}

 void TurnOffFona()
{
  //Serial.println("Turning off Fona ");
  while(digitalRead(FONA_PS)==HIGH)
  {
    digitalWrite(FONA_KEY, LOW);
    //delay(100);
    }
    digitalWrite(FONA_KEY, HIGH); 
}

int get_int_len (int value){
  int l=1;
  while(value>9){ l++; value/=10; }
  return l;
}

void sleepabit(int howlong)
  {
  int i2 = 0;  
  delay(100);  
  while (i2 < (howlong/8))
    {  
    cli();  
    delay(100); 
    // disable ADC
    //ADCSRA = 0;
    //prepare interrupts
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    // Set Watchdog settings:
    WDTCSR = (1<<WDIE) | (1<<WDE) | (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0);
    sei();
    //wdt_reset();  
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
    sleep_enable();
    // turn off brown-out enable in software
    //MCUCR = bit (BODS) | bit (BODSE);
    //MCUCR = bit (BODS); 
    sleep_cpu ();  
    // cancel sleep as a precaution
    sleep_disable();
    i2++;
    }
   wdt_disable(); 
  }
// watchdog interrupt
ISR (WDT_vect) 
{
  //i++;
   //Serial.println("waking up...");
}  // end of WDT_vect

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void flushSerial() {
    while (Serial.available()) 
    Serial.read();
}
