#include <SoftwareSerial.h>
#include <OneWire.h>

#define ssid        "SQUAD"
#define pass        ""

SoftwareSerial esp8266(18 ,19); // RX,TX

void setup(void)
{      
  Serial.begin(9600);
  esp8266.begin(9600);
  delay(5000);
 setupWifi();
}

void loop(void)
{
getWifiData();
}

void setupWifi()
{
  while(!resetModule()) {};
  //Serial.println("Module was successfully restarted");
  while(!connectWifi()) {};
  //Serial.println("Module was successfully Connected ");
  printIp();
  while(!setServer()) {};
  //Serial.println("Server Created");
}
  
boolean resetModule()
{
  //Serial.println("Resetting");
  esp8266.println("AT+RST");
  delay(1000);
  while(esp8266.available())
  {
  if(esp8266.find("ready"))
  {
    //Serial.println("Module is ready");
    return true;
  } else {
    //Serial.println("Module is unavailable. Restarting it.");
    return false;
  }
  }
}

boolean connectWifi()
{
  esp8266.println("AT+CWMODE=1");
  String cmd = "AT+CWJAP=\"";
  cmd+=ssid;
  cmd+="\",\"";
  cmd+=pass;
  cmd+="\"";
  esp8266.println(cmd);
  //Serial.println(cmd);
  while(!esp8266.available()){}
  delay(3000);

  if(esp8266.find("OK"))
  {
    //Serial.println("Connected to Wifi");
    while(!esp8266.available()){}
    if(esp8266.find("OK"))
    {  return true; }
    else
    {  return false; }
  }
  else  
  {
    //Serial.println("Cannot connect to WiFi. Trying again!");
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
    if(esp8266.find("OK"))    {  
      //Serial.println("Server Started"); 
    return true;   }
    else { return false; }
} 

// Get the data from the WiFi module and send it to the debug //Serial port
void receive(){
  delay(300);
  //Serial.println("-----RX-----");
  while (esp8266.available() >0) {
    Serial.write(esp8266.read());
  }
  //Serial.println();
}

void getWifiData()
{
  int index = 0;
  String message = "";
    while(esp8266.available()) // read the command character by character
    {
      delay(50);
        // read one character
      message+=char(esp8266.read());
    }
    if (!message.equals("")){
    Serial.print("Data = "+ message); // send the read character to the esp8266  
    }
  for (byte i = 1; i < message.length();i++)
  {
    if (message.charAt(i) == '-') {   index = i; }
  }
  
if (index > 0) {
  Serial.println("Found Hyphen");
  String tempRoom = message.substring(index-2,index);
  String tempRisk = message.substring(index+1);
  Serial.println("Room Number = " + tempRoom);
  Serial.println("Risk Level = " + tempRisk);
    ram();
  }

}
  
void printIp() {
esp8266.println("AT+CIFSR"); receive(); 
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
