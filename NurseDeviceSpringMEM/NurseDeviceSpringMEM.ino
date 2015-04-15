#include <avr/pgmspace.h>
#include <ST7565.h>
#include <SoftwareSerial.h>

#define ssid        "SQUAD"
#define pass        ""

SoftwareSerial esp8266(18 ,19);

//SETUP LCD DISPLAY
byte brightness = 175; byte rPin = 3; byte gPin = 5; byte bPin = 6;
//initialize RGB values for backlight
byte r = 0; byte g = 0; byte b = 0;

byte vPin = 16;
byte postSetup = 0;
//Previous Button Press Time
byte scrollPatientTime = 5; //Scroll Every 5 Seconds
int lastScroll = 0;
long lastVibrate = 0;
//Demo Data
byte maxQueue = 4;
char* nameQueue[] = {"","","",""};
int roomQueue[] = {0,0,0,0};
byte riskQueue[] = {0,0,0,0};
int timeQueue[] = {0,0,0,0};
byte queueSize = 0;

// Initialize Single Patient Values
//String name = ""; String rNum =""; byte riskLevel = 0; int patTime=0;

byte currentDisplayItem = 0;
long lastPatientTIme = 0;
byte timeoutSecs = 60; //Patient timeout Period secs
long lastLCDUpdate = 0; // Last Time LCD was updated
int lcdRefresh = 500; //Time btwn LCD update
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
  esp8266.begin(9600);
  ram();
  // Set Background of LCD to white
  // Initialize background color pins
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(17,OUTPUT);
   // Start vibration pin
  pinMode(vPin, OUTPUT);
  // Set Vibration pin off
  digitalWrite(vPin, LOW);
  digitalWrite(17,LOW);
  // Wait 2 seconds for device to start
  lcd.begin(0x1b);
  lcd.display();
  delay(3000);
  digitalWrite(17,HIGH);
  lcd.clear();
  ram();
  delay(500);
setupWifi();
Serial.println("Wifi Setup!");
  delay(500);
  postSetup = secs();
}

void loop(void)
{
processData();
updateDisplay();
}

void processData()
{
    if (queueSize > 0)  {  lcdRefresh = 200;   }
  else {   lcdRefresh = 350;  }
  checkVibrate();
  patientScroll();
    if (currentDisplayItem >= queueSize && queueSize > 0) 
  {  currentDisplayItem = 0;  }
    if (queueSize >= 1) {   updateDisplay();  }
    getWifiData();
}

void patientScroll()
{
  // Check if patient is timed out
  if (secs() - timeQueue[currentDisplayItem] > timeoutSecs && queueSize >=1)
  {
  removePatient(currentDisplayItem);
  }
  // Scroll if ready to scroll
  else if (secs() - lastScroll > scrollPatientTime) { lastScroll = secs();  currentDisplayItem++; }
}

void updateDisplay() 
// Update LCD display with proper patient data
{
  // Scroll through multiple alerts every LCD REFRESH ms. 
  if (millis() - lastLCDUpdate > lcdRefresh) 
  {  displayPatientInfo();    // Show patient data on LCD
    lastLCDUpdate = millis();
    }
}

void displayPatientInfo()
{
  lcd.clear();
  // printQueues("Displaying Patient Info");
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
  brightness = 200;
    setRed();
    break;
  }
  if (queueSize >= 1)  
 {  lcd.drawstring(0,0, "Secs: ");
    lcd.drawstring(30,0, intToChar(secs()));
    lcd.drawstring(0,1,"# of Alerts: ");
    lcd.drawstring(72,1, intToChar(queueSize));
    lcd.drawstring(0,4, concatString("Name: ", nameQueue[currentDisplayItem]));
    lcd.drawstring(0,5, "Room Number: ");
    lcd.drawstring(78,5,intToChar(roomQueue[currentDisplayItem])); 
    lcd.display();
    
}
else {
  lcd.clear();
  lcd.drawstring(0,0,intToChar(secs()));
  lcd.drawstring(0, 4, "No Patient Alerts");
  setOff();
  lcd.display();
}
}

void addPatient(char* patientName,int patRoomNumber,byte patRiskLevel)
{
  queueSize=queueSize+1;
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
  vibrate();
  printQueues("Added");
}

char* concatString(char* str1, char* str2)
{
  byte sumlen = strlen(str1) + strlen(str2);
  char temp[sumlen+2];
  String str = String(str1)+String(str2);
  str.toCharArray(temp,sumlen+2);
  return temp;
  }
  
void printQueues(char* header)
{
  Serial.println(header);
  for (byte i = 0; i <= maxQueue-1; i++)    {
    Serial.print(nameQueue[i]);
        Serial.print(",");
  }
  Serial.println();
  for (byte i = 0; i <= maxQueue-1; i++)    {
    Serial.print(roomQueue[i]);
        Serial.print(",");
  }
  Serial.println();
    for (byte i = 0; i <= maxQueue-1; i++)    {
    Serial.print(timeQueue[i]);
    Serial.print(",");
  }
  Serial.println();
}

void removePatient(byte index)
{
      for (byte j = index; j <= queueSize-1;j++) 
      {
        nameQueue[j] = nameQueue[j+1];
        roomQueue[j] = roomQueue[j+1];
        riskQueue[j] = riskQueue[j+1];
        timeQueue[j] = timeQueue[j+1];
      }

         nameQueue[queueSize-1] = "";
        roomQueue[queueSize-1] = 0;
        riskQueue[queueSize-1] = 0;
        timeQueue[queueSize-1] =0;
          queueSize--;
  printQueues("Deleted");
}

void changeRisk(byte index, byte newRisk)
{
 riskQueue[index] = newRisk;
}

boolean existsRoom(int room )
// Find index of patient in queue
{
  int index = 0;
  for (int i = 1; i <= queueSize; i++) {
    if (room == roomQueue[i])
    {
      return true;
    }
  }
return false;
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

char* intToChar(int num) 
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























void setupWifi()
{
  while(!resetModule()) {};
  Serial.println(F("Module was successfully restarted"));
  while(!connectWifi()) {};
  Serial.println(F("Module was successfully Connected"));
  printIp();
  while(!setServer()) {};
  Serial.println(F("Server Created"));
}

boolean resetModule()
{
  esp8266.println("AT+RST");
 
  while(esp8266.available())
  {
  if(esp8266.find("ready"))
  {
    Serial.println(F("Module is ready"));
    return true;
  } 
  else {
    Serial.println(F("Module is unavailable. Restarting it."));
        digitalWrite(17,LOW);
    delay(2000);
    digitalWrite(17,HIGH);
    return false;
  }
  }
}
boolean connectWifi()
{
  esp8266.println(F("AT+CWMODE=1"));
  delay(1000);
  String cmd = "AT+CWJAP=\"";
  cmd+="SQUAD";
  cmd+="\",\"";
  cmd+="";
  cmd+="\"";
  esp8266.println(cmd);
  Serial.println(cmd);
  while(!esp8266.available()){}
  delay(6000);

  if(esp8266.find("OK"))
  {
    Serial.println(F("Connected to Wifi"));
  return true; }
  else  
  {
    Serial.println(F("Cannot connect to WiFi. Trying again!"));
    delay(2500);
    return false;
  }
}
// to rx
boolean setServer()
{
    esp8266.println("AT+CIPMUX=1");
    receive();
    esp8266.println("AT+CIPSERVER=1,1100");
    // 1 = listening ---1100 = port number
    delay(500);
    while(!esp8266.available()){}
    if(esp8266.find("OK"))    {  Serial.println("Server Started"); return true;   }
    else { return false; }
} 

// Get the data from the WiFi module and send it to the debug serial port
void receive(){
  delay(300);
  while (esp8266.available() >0) {
    Serial.write(esp8266.read());
  }
  Serial.println();
}

void getWifiData()
  {
  byte indexHyphen = 0;
  byte indexBytes=0;
  String message = "";
  byte incomingByte = 0;
    while(esp8266.available()) // read the command character by character
    {
      delay(50);
        // read one character
      message+=char(esp8266.read());
    }
    if (!message.equals("")) {
    Serial.print("Data = "+ message); // send the read character to the esp8266  
    
  for (byte i = 1; i < message.length();i++)
  {
    if (message.charAt(i) == '-') {   indexHyphen = i; }
    if (message.charAt(i) == ':') { indexBytes = i-1;}
  }
  
if (indexHyphen > 0) {
  //Serial.println(F("Found Hyphen"));
  int roomChars = message.substring(indexBytes,indexBytes+1).toInt()-4;
 // Serial.println("Room Character = " + String(roomChars));
 String tempRoom = message.substring(indexHyphen-roomChars,indexHyphen);
 String tempRisk = message.substring(indexHyphen+1);
 Serial.println("Room = " + tempRoom);
  Serial.println(tempRoom);
  Serial.println(tempRisk);
  if (!existsRoom(tempRoom.toInt()))
  {
  if (tempRoom.toInt()==40){
  addPatient("Bob",tempRoom.toInt(),tempRisk.toInt());
  }
  if (tempRoom.toInt()== 705){
      addPatient("Charles",tempRoom.toInt(),tempRisk.toInt());
  }
    if (tempRoom.toInt()==8){
      addPatient("David",tempRoom.toInt(),tempRisk.toInt());
  }
  }
}
}
  }
  void checkVibrate()
  {
    if (millis() - lastVibrate > 1000)
  { digitalWrite(vPin, LOW); }
  }
  
  void vibrate()
  {
    lastVibrate = millis();
    digitalWrite(vPin, HIGH);
  }
void printIp() {
esp8266.println("AT+CIFSR"); receive(); 
}
