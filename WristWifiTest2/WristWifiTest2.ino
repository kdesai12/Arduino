#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

#define ssid   "SQUAD"
#define PASS ""
#define IP “192.168.0.3” //ESP8266 IP

int port = 1100;

SoftwareSerial esp8266(10,11);

void setup()
{
  pinMode(13,OUTPUT);
  pinMode(3,OUTPUT);
  digitalWrite(13,HIGH);
  digitalWrite(3,HIGH);
  Serial.begin(9600);
  esp8266.begin(9600);
  delay(5000);
  Serial.println("Test Started!");

  while(!resetModule());

  Serial.println("Module was successfully restarted");

  while(!connectWifi());

  while(!serverStart(port));
}

void loop()
{
}

boolean resetModule()
{
  esp8266.println("AT+RST");
  while(esp8266.available())
  {
  if(esp8266.find("ready"))
  {
    Serial.println("Module is ready");
    delay(1000);
    return true;
  } else {
    Serial.println("Module is unavailable. Restarting it.");
    digitalWrite(3,LOW);
    delay(2000);
    digitalWrite(3,HIGH);
    return false;
  }
  }
}

boolean connectWifi()
{
  esp8266.println("AT+CWMODE=3");
  String cmd = "AT+CWJAP=\"";
  cmd+=ssid;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  esp8266.println(cmd);
  Serial.println(cmd);
  while(!esp8266.available()){}
  delay(6000);
  if(esp8266.find("OK"))
  {
    Serial.println("Connected to Wifi");
    esp8266.println("AT+CIPMUX=1"); //set TCP to single connection
    while(!esp8266.available()){}
    if(esp8266.find("OK"))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    Serial.println("Cannot connect to WiFi. Trying again!");
    delay(2500);
    return false;
  }
}

boolean serverStart(int port)
{
  esp8266.flush();
  delay(1000);
  String cmd = "AT+CIPSERVER=1,";
  cmd+=port;
  esp8266.println(cmd);
  delay(500);
  Serial.println(cmd);
  while(esp8266.available())
  {
    if(esp8266.find("OK"))
    {
      Serial.println("Server is started!");
      return true;
    }
    else
    {
      Serial.println("Cannot create Server. Trying again!");
      delay(500);
      return false;
    }
  }
}
