#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystalFast.h>
#include <Button.h>

//SETUP LCD DISPLAY
int brightness = 255;
int rPin = 3;
int gPin = 5;
int bPin = 6;

int buttonPin1 = 9;
long lastPress1 = 0;
int buttonPin2 = 10;
long lastPress2 = 0;
//pins on LCD display
// lcd(4,5,6,14,13,12,11)
//    (RS,RW, EN, DB7, DB6, DB5, DB4)

int r = 0;
int g = 0;
int b = 0;

String nameQueue = "Albert,Bob,Carl123456789,Dick,Ed,";
String roomQueue = "1,2,3,4,5,";
String riskQueue = "1,2,3,4,4,";
String countQueue = "0,0,0,0,0,";

String name = "";
String rNum ="";
String riskStr = "";

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
Button button1 = Button(9);
Button button2 = Button(10);

void setup(void)
{
  Serial.begin(9600);
  SPI.begin();
  lcd.begin(16, 2);
// Initialize background color pins
pinMode(rPin, OUTPUT);
pinMode(gPin, OUTPUT);
pinMode(bPin, OUTPUT);
pinMode(buttonPin2, INPUT_PULLUP);
setOff();
}

void loop(void)
{
delay(150);

if (button1.isPressed()) { Serial.println("Pressed");} 
else {Serial.println("Not Pressed");}
}

//Functions
void testScript()
{
  
}
void button1Pressed()
{
  int buttonState = button1.isPressed();
  if ((buttonState == 1) && (millis() - lastPress1 > 5000) && (existsName("Joe") == false)) {
    nameQueue = addQueueEntry(nameQueue, "Joe", 1);
    roomQueue = addQueueEntry(roomQueue, "52", 1);
    riskQueue = addQueueEntry(riskQueue, "4", 1);
    lastPress1 = millis();
    printQueues("Button 1: Added Joe");
  }    
}

void button2Pressed()
{
  int buttonState = button2.isPressed();
    if ((buttonState == 1) && (millis() - lastPress2 > 5000) && (existsName("Steve") == false)) {
    nameQueue = addQueueEntry(nameQueue, "Steve", 1);
    roomQueue = addQueueEntry(roomQueue, "39", 1);
    riskQueue = addQueueEntry(riskQueue, "3", 1);
    lastPress2 = millis();
    printQueues("Button 2: Added Steve");
  }    
}

void updateDisplay() 
{
if (millis() - lastUpdate > lcdDelay) {
  if (currentDisplayItem > queueSize()) { 
    currentDisplayItem = 1; }
  getAllData(currentDisplayItem);
  displayPatientInfo(currentDisplayItem);
  Serial.println(currentDisplayItem);
  currentDisplayItem++;
  lastUpdate = millis();
  needUpdate = 0;
}

if (millis() - lastPress1 > 15000)
{
  removePatient("Joe");
  lastPress1 = millis();
}
}
void printQueues(String Header)
{
  Serial.println(Header);
  Serial.println(nameQueue);
  Serial.println(roomQueue);
  Serial.println(riskQueue);
}

void removePatient(String patientName)
{
  
  printQueues("Removing Patient ---> " + patientName);
  int nameIndex = findNameIndex(patientName);
  
 
 nameQueue = deleteQueueEntry(nameQueue, nameIndex);
roomQueue = deleteQueueEntry(roomQueue, nameIndex);
riskQueue = deleteQueueEntry(riskQueue,nameIndex);

printQueues("Removed Patient ---> " + patientName);

}

boolean existsName(String tempName)
{
  int s = queueSize();
  for (int i = 1; i < s; i++)
  {
    if (getQueueEntry(nameQueue, i) == tempName)
   {
       return true;
       break;
   }
  }
  
}
int findNameIndex(String tempName)
{
  printQueues("Finding Patient ---> " + tempName);
  int index = 0;
  for (int i = 1; i < queueSize(); i++) {
    if (tempName == getQueueEntry(nameQueue,i))
    {
      index = i;
    }
  }
  Serial.print("Found! ---> Index of " + tempName); Serial.println(index);
  return index;
}

void getAllData(int itemNumber)
{
  name = getQueueEntry(nameQueue, itemNumber);
  rNum = getQueueEntry(roomQueue, itemNumber);
  riskLevel = getQueueEntry(riskQueue, itemNumber).toInt();
}

String deleteQueueEntry(String queue, int itemNumber)
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
  return queue;
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
  temp = queue.substring(0,lastIndex+1);
  queue = temp + entry + "," + queue.substring(lastIndex+1, queue.length());
  return queue;
  
}

String moveQueueEntry(String queue, int itemNumber, int newItemNumber)
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
  int count = 0; 
  for (int x = 1; x < nameQueue.length(); x++) {
    if (nameQueue.charAt(x) == ',')
    {
    count++;
    }
  }
  return count;
}

void displayPatientInfo(int itemNumber) {
  lcd.clear();
 //lcd.home();
  lcdLine1 = formatString("Name: " + name);
  lcdLine2 = formatString("Room#: " + rNum);
  Serial.println(lcdLine1);
  Serial.println(lcdLine2);
  lcd.print(lcdLine1);
  lcd.setCursor(0,1);
  lcd.print(lcdLine2);
  
  switch (riskLevel) 
  {
  case 0:
  setOff();
  break;
  //Taken Care of
  case 1:
  setGreen();
  break;
  //Nurse Acknowledged
  case 2:
  setBlue();
  break;
  //Low Risk
  case 3:
  setYellow();
  break;
  //High Risk
  case 4: 
  setRed();
  break;
  }
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
analogWrite(rPin, 150);
analogWrite(gPin, 150);
analogWrite(bPin, 150);
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
