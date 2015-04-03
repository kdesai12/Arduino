#include <avr/pgmspace.h>
#include <Wire.h>
#include <ST7565.h>
#include <Button.h>

//SETUP LCD DISPLAY
byte brightness = 200;
byte rPin = 3;
byte gPin = 5;
byte bPin = 6;

byte postSetup = 0;
//Previous Button Press Time
byte scrollPatientTime = 5;
int lastScroll = 0;
//initialize RGB values for backlight
int r = 0;
int g = 0;
int b = 0;

//Demo Data
  char* nameQueue[] = {"Albert","Bob"};
int roomQueue[] = {11,22};
byte riskQueue[] = {3,4};
int timeQueue[] = {12,0};
byte queueSize = 2;

// Initialize Single Patient Values
String name = "";
String rNum ="";
byte riskLevel = 0;
int patTime=0;


byte currentDisplayItem = 1;
long lastPatientTIme = 0;
byte timeoutSecs = 10;; //Patient timeout Period secs
long lastLCDUpdate = 0; // Last Time LCD was updated
int lcdRefresh = 1250; //Time btwn LCD update
//// Patient  Timeout
int lastTimeoutCheck = 0; //last time patient timeout was checked

//Intialize LCD ***
// pin 12 - Serial data out (SID)
// pin 13 - Serial clock out (SCLK)
// pin 9 - Data/Command select (RS or A0)
// pin 11 - LCD reset (RST)
// pin 10 - LCD chip select (CS)

//ST7565 lcd(12, 13, 9, 11, 10);

void setup(void)
{      

  Serial.begin(9600);
  ram();
  // Set Background of LCD to white
  // Initialize background color pins
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);

  // Wait 2 seconds for device to start
 // lcd.begin(0x1b);
  //lcd.display();

  delay(10000);
  //lcd.clear();
    ram();

  delay(3000);
  postSetup = secs();
}

void loop(void)
{
  delay(12000);
  ram();
}

void addPatient(char* patientName,int patRoomNumber,byte patRiskLevel)
{
  
  queueSize++;
  for (byte i = queueSize; i>1; i--)
  {
    nameQueue[i] = nameQueue[i-1];
    roomQueue[i] =roomQueue[i-1];
    riskQueue[i] = riskQueue[i-1];
    timeQueue[i] = timeQueue[i-1];
  }
  nameQueue[0] = patientName;
  roomQueue[0] = patRoomNumber;
  riskQueue[0] = patRiskLevel;
  timeQueue[0] = secs();

}

void printQueues()
{
  
  for (byte i = 0; i < queueSize; i++)    {
    Serial.print(nameQueue[i]);
  }
  Serial.println();
  for (byte i = 0; i < queueSize; i++)    {
    Serial.print(roomQueue[i]);
  }
  Serial.println();
    for (byte i = 0; i < queueSize; i++)    {
    Serial.print(timeQueue[i]);
  }
  Serial.println();
}









int freeRam(void)
{
  extern int  __bss_end;
  extern int  *__brkval;
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
  }
  return free_memory;
}

void ram() { Serial.println("Free Ram --> " + String(freeRam()));  }
int secs() { return (int)(millis()/1000) - postSetup; }

