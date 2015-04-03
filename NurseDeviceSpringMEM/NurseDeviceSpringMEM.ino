#include <avr/pgmspace.h>
#include <Wire.h>
#include <ST7565.h>
#include <Button.h>

//SETUP LCD DISPLAY
byte brightness = 175; byte rPin = 3; byte gPin = 5; byte bPin = 6;
//initialize RGB values for backlight
byte r = 0; byte g = 0; byte b = 0;

byte postSetup = 0;
//Previous Button Press Time
byte scrollPatientTime = 4; //Scroll Every 5 Seconds
int lastScroll = 0;

//Demo Data
char* nameQueue[] = {"Albert","Bob","Carl","Dick",""};
byte roomQueue[] = {11,22,33,44,0};
byte riskQueue[] = {3,4,3,4,0};
int timeQueue[] = {40,0,20,30,0};
byte queueSize = 3;

// Initialize Single Patient Values
//String name = ""; String rNum =""; byte riskLevel = 0; int patTime=0;

byte currentDisplayItem = 0;
long lastPatientTIme = 0;
byte timeoutSecs = 20; //Patient timeout Period secs
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

ST7565 lcd(12, 13, 9, 11, 10);

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
   lcd.begin(0x1b);
  lcd.display();
  delay(5000);
  //lcd.clear();
  ram();

  delay(3000);
  postSetup = secs();
}

void loop(void)
{
  ram();
processData();
ram();
updateDisplay();
}

void processData()
{
  ram();
    if (queueSize > 0)  {  lcdRefresh = 200;   }
  else {   lcdRefresh = 350;  }
  patientScroll();
    if (currentDisplayItem > queueSize && queueSize > 0) 
  {  currentDisplayItem = 0;  }
    if (queueSize >= 1) {  updateDisplay();  }
    ram();
}

void patientScroll()
{
  // Check if patient is timed out
  if (secs() - timeQueue[currentDisplayItem] > timeoutSecs && queueSize >=1)
  {
    ram();
  removePatient(currentDisplayItem); 
  ram();
  }
  // Scroll if ready to scroll
  else if (secs() - lastScroll > scrollPatientTime) { lastScroll = secs();  currentDisplayItem++; }
}

void updateDisplay() 
// Update LCD display with proper patient data
{
  // Scroll through multiple alerts every LCD REFRESH ms. 
  if (millis() - lastLCDUpdate > lcdRefresh) 
  {  ram(); displayPatientInfo();    // Show patient data on LCD
    lastLCDUpdate = millis();
    ram();     }
}

void displayPatientInfo()
{
  lcd.clear();
  ram();
  printQueues("Displaying Patient Info");
  if (queueSize >= 1)  
 {  lcd.drawstring(0,0, "Secs: ");
    lcd.drawstring(30,0, intToChar(secs()));
    lcd.drawstring(0,1,"# of Alerts: ");
    lcd.drawstring(72,1, intToChar(queueSize));
    lcd.drawstring(0,4, concatString("Name: ", nameQueue[currentDisplayItem]));
    lcd.drawstring(0,5, "Room Number: ");
    lcd.drawstring(78,5,intToChar(roomQueue[currentDisplayItem])); 
    ram();
    lcd.display();
    ram();
    switch (riskQueue[currentDisplayItem])
  {
    //Taken Care of - GREEN
  case 1:
  brightness = 0;
    setOff();
    break;
    //Nurse Acknowledged - BLUE
  case 2:
  brightness = 200;
    setBlue();
    break;
    //Low Risk - YELLOW
  case 3:
  brightness = 250;
    setYellow();
    break;
    //High Risk - RED
  case 4:
  brightness = 250;
    setRed();
    break;
  }
}
else {
  lcd.clear();
  Serial.println(F("No Alerts"));
  lcd.drawstring(0,0,intToChar(secs()));
  lcd.drawstring(0, 1, "No Patient Alerts");
  lcd.display();
}
}

void addPatient(char* patientName,int patRoomNumber,byte patRiskLevel)
{
  printQueues("Adding");
  queueSize++;
  for (byte i = queueSize; i>0; i--)
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
  printQueues("Added");
}

char* concatString(char* str1, char* str2)
{
  ram();
  byte sumlen = strlen(str1) + strlen(str2);
  char temp[sumlen+2];
  String str = String(str1)+String(str2);
  str.toCharArray(temp,sumlen+2);
  return temp;
  }
  
void printQueues(char* header)
{
  Serial.println(header);
  for (byte i = 0; i < queueSize-1; i++)    {
    Serial.print(nameQueue[i]);
        Serial.print(",");
  }
  Serial.println();
  for (byte i = 0; i < queueSize-1; i++)    {
    Serial.print(roomQueue[i]);
        Serial.print(",");
  }
  Serial.println();
    for (byte i = 0; i < queueSize-1; i++)    {
    Serial.print(timeQueue[i]);
    Serial.print(",");
  }
  Serial.println();
}

void removePatient(byte index)
{
      for (byte j = index; j < queueSize-1;j++) 
      {
        nameQueue[j] = nameQueue[j+1];
        roomQueue[j] = roomQueue[j+1];
        riskQueue[j] = riskQueue[j+1];
        timeQueue[j] = timeQueue[j+1];
      }
  queueSize--;
  printQueues("Deleted");
}

void changeRisk(byte index, byte newRisk)
{
 riskQueue[index] = newRisk;
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

char* intToChar(byte num) 
{
  String str = String(num);
  char result[4] ;
  str.toCharArray(result, 4);
  return result;
}
void setOff()
{
  digitalWrite(rPin, HIGH);
  digitalWrite(gPin, HIGH);
  digitalWrite(bPin, HIGH);
}

void setRed()
{
  setBG(255, 0, 0, brightness);
}

void setYellow()
{
  setBG(250, 225, 5, brightness);
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
void setBG(byte r, byte g, byte b, byte brightness) 
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
