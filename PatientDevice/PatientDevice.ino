#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

RF24 radio(9,10);
const uint64_t pipe1 = 0xF0F0F0F0E1LL;

void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(10,20);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(5);
  radio.openWritingPipe(pipe1);
}

void loop(void)
{
  sendData("Message 1");
  delay(5500);
  sendData("Message 2");
  delay(5500);
}

void sendData(String data)
{
  radio.startListening();
  int messageSize = data.length();
  int charToSend[1];
  int sent = 0;
  for (int i = 0; i < messageSize; i++)
  {
    sent = 0;
    charToSend[0] = data.charAt(i);
    Serial.println(charToSend[0]);
    while (sent == 0)
    {
      Serial.println("Sent = 0");
      sent = radio.write(charToSend, 1);
    }
    Serial.println("Char Sent!");
  }
  char EndChar = '9';
  radio.write(EndChar,1);
  
  radio.powerDown();
  delay(1000);
  radio.powerUp();
}



