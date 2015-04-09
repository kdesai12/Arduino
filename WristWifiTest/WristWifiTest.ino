#include <SoftwareSerial.h>
#include <OneWire.h>

#define SSID        "MyHomeSSID"
#define PASS        "MyPassword"
#define TARGET_IP   "192.168.1.xx"
#define TARKET_PORT 5000

SoftwareSerial wifi(2,3); // RX,TX
int incomingByte=0;

void setup(void)
{      
  Serial.begin(9600);
  wifi.begin(9600);
  
  joinNetwork();
  
  wifi.println("AT+CIPMUX=0");
  receive();
  
}


void loop(void)
{
  
}

void joinNetwork()
{
  wifi.println("AT+CWMODE=3");
 receive();
  Serial.println("Joining SQUAD");
  wifi.print("AT+CWJAP=");
  wifi.print(SSID);  
  wifi.print(",");
  wifi.print(PASS);
  wifi.println("");
  receive();
  delay(2500);
  Serial.println("Connected to Squad");
  wifi.println("AT+CWJAP?");
  receive();
  printiP();

  
}

void resetWifi();{
  wifi.println("AT+RST");
  receive();
}
// to tx
void setClient()
{
  wifi.println("AT+CIPMUX=1");
  receive();
  wifi.println("AT+CIPSTART=4,\"TCP\", \"IPADDRESSHERE\",80"); // 4 = channel; 80 = port
  receive();
}
// to rx
void setServer()
{
    wifi.println("AT+CIPMUX=1");
    receive();
    printIp();
    wifi.println("AT+CIPSERVER=1,80"); // 1 = listening ---1336 = port number
    receive();
}

// Get the data from the WiFi module and send it to the debug serial port
void receive(){
  delay(300);
  Serial.println("-----RX-----");
  while (wifi.available() >0) {
    incomingByte = wifi.read();
    Serial.write(incomingByte);
  }
  Serial.println();
}

void sendData(String channel, String data)
{
// Send the data to the WiFi module
  wifi.print("AT+CIPSEND=");
  wifi.print(channel);
  wifi.print(",");
  wifi.println(data.length());
  delay(500);
  wifi.print(data);
  receive();
}

void printIp() {
wifi.println("AT+CIFSR"); receive(); 
}
