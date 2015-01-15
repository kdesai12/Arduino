#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "LiquidCrystal.h"
#include "Wire.h"

//SETUP LCD DISPLAY

const rPin 3
const gPin 5
const bPin 6

LiquidCrystal lcd(8,4,14,15,16,17)

int redBG[3] = {255, 50, 50}; // RGB values for red background
int greenBG[3] = {50, 255, 50}; 
int yellowBG[3] = {255, 255, 50};
int blueBG[3] = {50, 50, 255};

int r = 0;
int g = 0;
int b = 0;

//SETUP RF RADIO

RF24 radio(9,10); // CEpin, CSpin
const uint64_t pipe = 0xE8E8F0F0E1LL; // Recieving pipe 
int riskLevel = -1; 

void setup() {
  
Serial.begin(9600);

Serial.println("RF Radio Starting");
radio.begin();

// set up the LCD's number of rows and columns:
lcd.begin(16, 2);

//Recieving Pipe
radio.openReadingPipe(1,pipe)
radio.startListening();
// Print a message to the LCD.
lcd.print("RGB 16x2 Display "); //16 Characters Wide
lcd.setCursor(0,1); //Next Row
lcd.print(" Multicolor LCD ");

// Initialize background color
pinMode(rPin, OUTPUT);
pinMode(gPin, OUTPUT);
pinMode(bPin, OUTPUT);
}



void loop() 
{
  setBG(redBG, 200);
  delay(1500) //Wait 1500 milliseconds
  setBG(yellowBG,200);
  delay(1500) 
  setBG(blueBG,200);
}


void setBG(int Array, int brightness) 
{
//Store RGB values 
r = Array[1];
b = Array[2];
g = Array[3];

//Map to account for increased brightness of RED led
r = map(r, 0, 255, 0, 100);
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
