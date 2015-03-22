#include <SPI.h>
#include <Wire.h>
#include <ST7565.h>
#include <Button.h>

//SETUP LCD DISPLAY
int brightness = 200;
int rPin = 3;
int gPin = 5;
int bPin = 6;

int vPin = 14;
//Previous Button Press Time
long lastPress1 = 0;
long lastPress2 = 0;

//Last Vibrate Time
long lastVibrate = 0;
int vibTime = 500;
//initialize RGB values for backlight
int r = 0;
int g = 0;
int b = 0;

//Demo Data
String nameQueue = "Albert,Bob,Carl123456789,Dick,Ed,";
String roomQueue = "1,2,3,4,5,";
String riskQueue = "3,4,3,4,4,";
String timeQueue = "";
// Initialize Data Queues
//String nameQueue = "";
//String roomQueue = "";
//String riskQueue = "";
//String timeQueue = "";

// Initialize Single Patient Values
String name = "";
String rNum ="";
int riskLevel = 0;
float patTime=0;

int currentDisplayItem = 1;
int timeoutMins = 10; //Patient timeout Period Mins
long lastLCDUpdate = 0; // Last Time LCD was updated
int lcdDelay = 750;; //Time btwn LCD update
// Patient  Timeout
int timeoutInterval = 15000; //check if patients are timed out every XX ms
long lastTimeoutCheck = 0;//last time patient timeout was checked

//Intialize LCD ***
// pin 12 - Serial data out (SID)
// pin 13 - Serial clock out (SCLK)
// pin 9 - Data/Command select (RS or A0)
// pin 11 - LCD reset (RST)
// pin 10 - LCD chip select (CS)
ST7565 lcd(12, 13, 9, 11, 10);

void setup(void)
{
  // Set Background of LCD to white
  setOff();
  // Initialize background color pins
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  // Start vibration pin
  pinMode(vPin, OUTPUT);
  // Set Vibration pin off
  digitalWrite(vPin, LOW);


  // Wait 2 seconds for device to start
  lcd.begin(0x1b);
  delay(2000);
}

void loop(void)
{
  if (queueSize() > 0) 
  { lcdDelay = 750; }
  else
  { lcdDelay = 1250; }
  
  updateDisplay();
  // Check vibration status AND 
  //turn off if vibraion has occured for > 750ms
  if (millis() - lastVibrate > (long) vibTime)
  { digitalWrite(vPin, LOW); }
}

//Functions
void addPatient(String patientName, int patRoomNumber, int patRiskLevel) 
//Add Patient Information to proper queues
{
  nameQueue = addQueueEntry(nameQueue, patientName, 1);
  roomQueue = addQueueEntry(roomQueue, String(patRoomNumber), 1);
  riskQueue = addQueueEntry(riskQueue, String(patRiskLevel), 1);
  timeQueue = addQueueEntry(timeQueue, String(float(millis()/1000)),1);
}

void vibrate()
//Vibrate every 3s
{
  if (millis() - lastVibrate > 2500) {
  digitalWrite(vPin, HIGH);
  lastVibrate = millis(); }
}

void updateDisplay() 
// Update LCD display with proper patient data
{
  // Scroll through multiple alerts every LCD DELAY ms. 
  if (millis() - lastLCDUpdate > lcdDelay) {
  //Get patient data @ CURRENT DISPLAY ITEM index 
  // Show patient data on LCD
  displayPatientInfo(currentDisplayItem);
  // Iterate through multiple alerts
  currentDisplayItem++;
  lastLCDUpdate = millis();
}
}

void processData()
{
   patientTimeout();
   if (currentDisplayItem > queueSize()) 
   { 
  // 
      currentDisplayItem = 1; 
   }

   
}
  
void removePatient(int itemNumber)
// Remove patient from queue
{
  nameQueue = deleteQueueEntry(nameQueue, itemNumber);
  roomQueue = deleteQueueEntry(roomQueue, itemNumber);
  riskQueue = deleteQueueEntry(riskQueue, itemNumber);
  timeQueue = deleteQueueEntry(timeQueue, itemNumber);
}

void patientTimeout()
{
  if (millis() - lastTimeoutCheck > (long)timeoutInterval) {
  for (int x = 1; x < nameQueue.length(); x++ ){
  if (getQueueEntry(timeQueue, x).toFloat() - (float)(millis()/1000) < (float)(-timeoutMins*60))
  {
    removePatient(x);
  }
  }
  lastTimeoutCheck = millis();
  }
}

void getAllData(int itemNumber)
// Get all patient data by using index number
{
  name = getQueueEntry(nameQueue, itemNumber);
  rNum = getQueueEntry(roomQueue, itemNumber);
  riskLevel = getQueueEntry(riskQueue, itemNumber).toInt();
  patTime = getQueueEntry(timeQueue, itemNumber).toFloat();
}

String deleteQueueEntry(String queue, int itemNumber)
// Delete individual queue entries by using index number
{ 
  if (itemNumber > 0)
  {
  itemNumber--;
  int count = 0;
  int lastIndex = -1;
  while (count < itemNumber) 
  {
    count++;
    lastIndex = queue.indexOf(',',lastIndex+1); 
  }
  queue.remove(lastIndex+1, queue.indexOf(',',lastIndex+1) - lastIndex);
  }
    return queue; // returns new queue after deleting item
}

//Complete
String getQueueEntry(String queue, int itemNumber)
{
  itemNumber--;
  int count = 0;
  int lastIndex = -1;
  while (count < itemNumber) 
  {
    count++;
    lastIndex = queue.indexOf(',',lastIndex+1); 
  }
  return queue.substring(lastIndex+1,queue.indexOf(',',lastIndex+1));
}

//Complete
void changeRiskQueue(int itemNumber, char newRisk)
{
  itemNumber--;
  int count = 0;
  int lastIndex = 0;
  
  while (count < itemNumber) 
  {
    count++;
    lastIndex = riskQueue.indexOf(',',lastIndex+1); 
  }
  riskQueue.setCharAt(lastIndex+1,newRisk);
}

//Complete
String addQueueEntry(String queue, String entry, int itemNumber) 
{
  itemNumber--;
  String temp = "";
  int count = 0;
  int lastIndex = 0;
  while (count < itemNumber) 
    {
      count++;
      lastIndex = queue.indexOf(',',lastIndex+1); 
    }
  if (itemNumber > 0) 
    {
      temp = queue.substring(0,lastIndex+1);
      queue = temp + entry + "," + queue.substring(lastIndex+1, queue.length());
    }
  else if (itemNumber == 0) 
    {
      queue = entry + "," + queue;   
    }
  return queue;
}

String moveQueueEntry(String queue, int itemNumber, int newItemNumber)
// Can be used to sort queues by moving items from itemNumber to new ItemNumber
{
 String temp = getQueueEntry(queue, itemNumber);
 if (itemNumber > newItemNumber) 
 {
 queue = addQueueEntry(queue,temp, newItemNumber);
 queue = deleteQueueEntry(queue, newItemNumber);
 queue = deleteQueueEntry(queue, itemNumber+1);
 }
 if (itemNumber < newItemNumber) 
 {
 queue = deleteQueueEntry(queue, itemNumber);
 queue = addQueueEntry(queue,temp, newItemNumber);
 queue = deleteQueueEntry(queue, newItemNumber);
 }
 return queue;
}

int queueSize() {
  //Finds number of names in  nameQueue 
  int count = 0; 
  for (int x = 1; x < nameQueue.length(); x++) {
    if (nameQueue.charAt(x) == ',')
    {
    count++;
    }
  }
  return count;
}
// ***
void displayPatientInfo(int itemNumber) {
  // Displays and formats data to LCD Display
 lcd.clear();
 getAllData(currentDisplayItem);

 // When there are alerts to be shown
 if (queueSize() >= 1) {
   String S1 = "Name: " + name;
   String S2 = "Room#: " + rNum;
    char lcdLine1[S1.length()];
    char lcdLine2[S2.length()];
    S1.toCharArray(lcdLine1,S1.length());
    S2.toCharArray(lcdLine2,S2.length());
    vibrate();
    lcd.drawstring(0, 1, lcdLine1);
    lcd.drawstring(0, 2, lcdLine2);
 }
// When there are no alerts
 else {
  char lcdLine1[] = "No Patient Alerts";
  lcd.drawstring(0, 1, lcdLine1);
  setOff();
  }
  // Print Queue Size
  lcd.drawchar(100,0,char(queueSize()));
  
  // Set background based on riskLevel
  switch (riskLevel) 
  {
  //Taken Care of - GREEN
  case 1:
  setGreen(); 
  break;
  //Nurse Acknowledged - BLUE
  case 2:
  setBlue();
  break;
  //Low Risk - YELLOW
  case 3:
  setYellow();
  break;
  //High Risk - RED
  case 4: 
  setRed();
  break;
  }
}

void setOff()
{
analogWrite(rPin, 0);
analogWrite(gPin, 0);
analogWrite(bPin, 0);
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

int existsPatient(int room)
//See if patient is in Queue (Found by room number)
{
  int i = 1;
  int exPat = 0;
  while (i <= queueSize() && exPat == 0)
  {
    if (getQueueEntry(roomQueue, i).equals(String( room)))
   {
       exPat = 1;
   }
   i++;
  }
 return exPat;
}

int findRoomIndex(int room )
// Find index of patient in queue
{
  printQueues("Finding Patient ---> " + String(room));
  int index = 0;
  for (int i = 1; i <= queueSize(); i++) {
    if (room == getQueueEntry(roomQueue,i).toInt())
    {
      index = i;
    }
  }
  Serial.print("Found! ---> Index of " + String(room)); Serial.println(index);
  return index;
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

void printQueues(String Header)
{
  Serial.println(Header);
  Serial.println(nameQueue);
  Serial.println(roomQueue);
  Serial.println(riskQueue);
}
