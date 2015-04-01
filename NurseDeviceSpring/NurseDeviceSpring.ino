
#include <Wire.h>
#include <ST7565.h>
#include <Button.h>

//SETUP LCD DISPLAY
byte brightness = 200;
int rPin = 3;
int gPin = 5;
int bPin = 6;

int postSetup = 0;
//Previous Button Press Time
int scrollPatientTime = 3;
int lastScroll = 0;
//initialize RGB values for backlight
int r = 0;
int g = 0;
int b = 0;

//Demo Data
String nameQueue = "Albert,Bob,";
String roomQueue = "11,22,";
String riskQueue = "3,4,";
String timeQueue = "12,0,";

// Initialize Data Queues
//String nameQueue = "";
//String roomQueue = "";
//String riskQueue = "";
//String timeQueue = "";

// Initialize Single Patient Values
String name = "";
String rNum ="";
byte riskLevel = 0;
int patTime=0;

byte currentDisplayItem = 1;
long lastPatientTIme = 0;
byte timeoutSecs = 12; //Patient timeout Period secs
long lastLCDUpdate = 0; // Last Time LCD was updated
int lcdRefresh = 1250; //Time btwn LCD update
// Patient  Timeout
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
  // Set Background of LCD to white
  setOff();
  // Initialize background color pins
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);

  // Wait 2 seconds for device to start
  lcd.begin(0x1b);
  lcd.display();
  delay(8000);
  lcd.clear();
  ram();
  Serial.println(getQueueEntry(nameQueue,1));
  delay(3000);
  postSetup = secs();
}

void loop(void)
{
  processData();
  updateDisplay();
}

void displayPatientInfo() {
  // Displays and formats data to LCD Display
  lcd.clear();
  // When there are alerts to be shown
  printQueues(String(queueSize()));
  if (queueSize() >= 1) {
    String S1 = "Name: " + name;
    String S2 = "Room#: " + rNum;
    String S3 = "Secs:";
    S3 += String(secs());
    S3 += "  # Alerts: " ;
    S3 += String(queueSize());
    char lcdLine1[S1.length()+1];
    char lcdLine2[S2.length()+1];
    char lcdQueueSize[S3.length()+1];
    S1.toCharArray(lcdLine1, S1.length()+1);
    S2.toCharArray(lcdLine2, S2.length()+1);
    S3.toCharArray(lcdQueueSize, S3.length()+1);
    lcd.drawstring(0, 0, lcdQueueSize);
    lcd.drawstring(0, 1, lcdLine1);
    lcd.drawstring(0, 2, lcdLine2);
    lcd.display();
  }
  // When there are no alerts
  else {
    lcd.clear();
    Serial.println("No Alerts");
    char lcdLine1[18] = "No Patient Alerts";
    lcd.drawstring(0, 1, lcdLine1);
    lcd.display();
    name = "";
    rNum ="";
    riskLevel = 1;
    patTime=0;
  }
  // Set background based on riskLevel
  switch (riskLevel)
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
  brightness = 200;
    setRed();
    break;
  }
}

void processData()
{
  if (queueSize() > 0)  {  lcdRefresh = 500;   }
  else {   lcdRefresh = 1000;  }
  
  if ((secs() - getQueueEntry(timeQueue, currentDisplayItem).toInt() > timeoutSecs) && (queueSize() >= 1))
      { 
        Serial.println("Removing Patient " + String(currentDisplayItem));
        removePatient(currentDisplayItem); 
      }
      
  else if (secs() - lastScroll > scrollPatientTime) {lastScroll = secs();  currentDisplayItem++;  }
  
  if (currentDisplayItem > queueSize() && queueSize() > 0) 
  {  currentDisplayItem = 1;  }
  
  if (queueSize() >= 1) {  getAllData(currentDisplayItem);  }
}


void updateDisplay() 
// Update LCD display with proper patient data
{
  long begtime = millis();
  // Scroll through multiple alerts every LCD REFRESH ms. 
  if (millis() - lastLCDUpdate > lcdRefresh) {
    // Show patient data on LCD
    displayPatientInfo();
    lastLCDUpdate = millis();
    ram();
  }
}

//void patientTimeout() {
//  // 2000 ms Timeout check interval
//  long begtime = millis();
//  if (secs() - lastTimeoutCheck > 5) {
//    Serial.println("Checking Timeout");
//    for (int x = 1; x < queueSize(); x++ ){
//      Serial.println(getQueueEntry(timeQueue, x));
//      if (getQueueEntry(timeQueue, x).toInt() - secs() < timeoutSecs)
//      { 
//        Serial.println("Removing Patient " + String(x));
//        removePatient(x); 
//      }
//    }
//    lastTimeoutCheck = secs();
//  }
//  //Serial.println("Time to Timeout Patients = "+ String(millis() - begtime));
//}

void addPatient(String patientName, int patRoomNumber, int patRiskLevel) 
//Add Patient Information to proper queues
{
  nameQueue = addQueueEntry(nameQueue, patientName, 1);
  roomQueue = addQueueEntry(roomQueue, String(patRoomNumber), 1);
  riskQueue = addQueueEntry(riskQueue, String(patRiskLevel), 1);
  timeQueue = addQueueEntry(timeQueue, String(secs()),1);
}
void removePatient(int itemNumber)
// Remove patient from queue
{
  //printQueues("Patient " + String(itemNumber) + " Being Removed");
  nameQueue = deleteQueueEntry(nameQueue, itemNumber);
  roomQueue = deleteQueueEntry(roomQueue, itemNumber);
  riskQueue = deleteQueueEntry(riskQueue, itemNumber);
  timeQueue = deleteQueueEntry(timeQueue, itemNumber);
  //printQueues("Patient " + String(itemNumber) + " Removed");
}

void getAllData(int itemNumber)
// Get all patient data by using index number
{
  name = getQueueEntry(nameQueue, itemNumber);
  rNum = getQueueEntry(roomQueue, itemNumber);
  riskLevel = getQueueEntry(riskQueue, itemNumber).toInt();
  patTime = getQueueEntry(timeQueue, itemNumber).toInt();
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
    if (getQueueEntry(roomQueue, i).equals(String(room))){
      exPat = 1; 
    }
    i++;
  }
  return exPat;
}

int findRoomIndex(int room )
// Find index of patient in queue
{
  int index = 0;
  for (int i = 1; i <= queueSize(); i++) {
    if (room == getQueueEntry(roomQueue,i).toInt())
    {
      index = i;
    }
  }
  return index;
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

void printQueues(String Header)
{
  Serial.println(Header);
  Serial.println(nameQueue);
  Serial.println(roomQueue);
  Serial.println(riskQueue);
  Serial.println(timeQueue);
}

int secs() { return (int)(millis()/1000) - postSetup; }

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

