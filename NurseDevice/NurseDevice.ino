#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystalFast.h>
#include <RF24_config.h>

RF24 radio(9,10);
const uint64_t pipe1 = 0xF0F0F0F0E1LL;
int msg[1];
int lastmsg = 1;
String theMessage = "";
//SETUP LCD DISPLAY
int brightness = 255;
int rPin = 3;
int gPin = 5;
int bPin = 6;
int buttonPin = 19;
//pins on LCD display
// lcd(4,5,6,14,13,12,11)
//    (RS,RW, EN, DB7, DB6, DB5, DB4)

int r = 0;
int g = 0;
int b = 0;


String lcdLine1 = "";
String lcdLine2 = "";


byte needUpdate = 1;
byte screenOn = 1;
int riskLevel = 1;
int currentDisplayItem = 1;
long lastUpdate = 5000;
long lastLoop = 0;
int lcdDelay = 5500; //Milliseconds

// Pins on Trinket
LiquidCrystalFast lcd(8,14,4,18,17,16,15);

void setup(void)
{
  //Serial.begin(9600);
  radio.begin();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(10,20);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(5);
  radio.openReadingPipe(1,pipe1);
  radio.startListening();
  lcd.begin(16, 2);
// Initialize background color pins
pinMode(rPin, OUTPUT);
pinMode(gPin, OUTPUT);
pinMode(bPin, OUTPUT);
pinMode(buttonPin, INPUT);
setYellow();

}

void loop(void)
{
  getData();
}

void getData()
{
  if (radio.available()){
    boolean done = false;  
      done = radio.read(msg, 1); 
      char theChar = msg[0];
      if (msg[0] != 2){
        theMessage.concat(theChar);
        }
      else {
       printLCD(theMessage);
       theMessage= ""; 
      }
   }
    radio.powerDown();
    delay(1000);
    radio.powerUp();
}

void printLCD(String str) 
{
  
  lcd.clear();
  String lcdLine1 = formatString(str);
  lcd.print(lcdLine1);
  delay(100);
  
}


String formatString(String Str)
{
  if (Str.length() > 16)
  {
    return Str.substring(0,15);
  }
  else 
  {
    return expand16(Str);
  }
}
String expand16(String Str) {
  while (Str.length() < 16) 
  {
    Str += " ";
  }
  return Str;
}

void setOff()
{
analogWrite(rPin, 255);
analogWrite(gPin, 255);
analogWrite(bPin, 255);
}

void setRed()
{
  setBG(255, 10, 0, brightness);
}

void setYellow()
{
  setBG(255, 200, 5, brightness);
}

void setBlue()
{
  setBG(50, 75, 255, brightness);
}
void setGreen()
{
  setBG(0, 255, 0, brightness);
}

//Complete
void setBG(int r, int g, int b, int brightness) 
{
//Map to account for increased brightness of RED led
r = map(r, 0, 255, 0, 120);
g = map(g, 0, 255, 0, 150);

// Map to achieve correct display brightness
r = map(r, 0, 255, 0, brightness);
g = map(g, 0, 255, 0, brightness);
b = map(b, 0, 255, 0, brightness);

// Invert RGB values from 0-255 to 255-0 due to display feature
r = map(r, 0, 255, 255, 0);
g = map(g, 0, 255, 255, 0);
b = map(b, 0, 255, 255, 0);

analogWrite(rPin, r);
analogWrite(gPin, g);
analogWrite(bPin, b);
}
