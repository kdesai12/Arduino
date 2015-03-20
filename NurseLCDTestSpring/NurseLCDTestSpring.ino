#include "ST7565.h"

// the LCD backlight is connected up to a pin so you can turn it on & off
int rPin = 3;
int gPin = 5;
int bPin = 6;

int brightness = 150;
//initialize RGB values for backlight
int r = 0;
int g = 0;
int b = 0;

// pin 12 - Serial data out (SID)
// pin 13 - Serial clock out (SCLK)
// pin 9 - Data/Command select (RS or A0)
// pin 11 - LCD reset (RST)
// pin 10 - LCD chip select (CS)
ST7565 glcd(12, 13, 9, 11, 10);

// The setup() method runs once, when the sketch starts
void setup()   {                
  Serial.begin(9600);
delay(5000); 
  Serial.print(freeRam());
  
  // turn on backlight
    // Initialize background color pins
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  setGreen();
  // initialize and set the contrast to 0x18
  glcd.begin(0x1b);

  glcd.display(); // show splashscreen
  Serial.print(freeRam());
  delay(2000);
  glcd.clear();

  // draw a string at location (0,0)
  glcd.drawstring(0, 0, "LCD Works!");
  glcd.display();
  delay(2000);
  glcd.clear();
}


void loop()                     
{
  glcd.drawstring(0, 0, "red");
  setRed();
  glcd.display();
  delay(2500);
  glcd.clear();
  
    glcd.drawstring(0, 0, "Blue");
  setBlue();
  glcd.display();
  delay(2500);
  glcd.clear();
  
    glcd.drawstring(0, 0, "Yellow");
  setYellow();
  glcd.display();
  delay(2500);
  glcd.clear();
}

// this handy function will return the number of bytes currently free in RAM, great for debugging!   
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
} 

void setBG(int r, int g, int b, int brightness) 
{
//Map to account for increased brightness of RED led
r = map(r, 0, 255, 0, 200);
g = map(g, 0, 255, 0, 200);

// Map to achieve correct display brightness
r = map(r, 0, 255, 0, brightness);
g = map(g, 0, 255, 0, brightness);
b = map(b, 0, 255, 0, brightness);

// Invert RGB values from 0-255 to 255-0 due to display feature (LCD dependent)
r = map(r, 0, 255, 255, 0);
g = map(g, 0, 255, 255, 0);
b = map(b, 0, 255, 255, 0);

// Output proper RGB values for BG color
analogWrite(rPin, r);
analogWrite(gPin, g);
analogWrite(bPin, b);
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


