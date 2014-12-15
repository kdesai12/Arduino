#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <RF24Network.h>

RF24 radio(9,10);



// Network uses that radio
RF24Network network(radio);

// Address of our node
const uint16_t this_node = 1;

// Address of the other node
const uint16_t other_node = 0;

const uint64_t pipe1 = 0xE8E8F0F0E1LL;
// How often to send 'hello world to the other unit
const unsigned long interval = 2000; //ms

// When did we last send?
unsigned long last_sent;
void setup(void)
{
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  //radio.begin();
  // optionally, increase the delay between retries & # of retries
  //radio.setRetries(15,20);
  //radio.setChannel(100);

  //radio.openWritingPipe(pipe1);

  //delay(10000);

}

void loop(void)
{
  sendDataNetwork();

}
void sendDataNetwork() {
  // Pump the network regularly
  network.update();

  // If it's time to send a message, send it!
  unsigned long now = millis();
  if ( now - last_sent > interval  )
  {
    last_sent = now;

    //toggleLED();
    printf("Sending...\r\n");
    const char* hello = "Hello, world!";
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header,hello,strlen(hello));
    if (ok)
      printf("\tok.\r\n");
    else
    {
      printf("\tfailed.\r\n");
      delay(250); // extra delay on fail to keep light on longer
    }
    //toggleLED();
  }
}

void sendData(int patientNumber)
{
radio.stopListening();
  //Serial.println(patientNumber);
int sent = radio.write(&patientNumber,sizeof(int));
Serial.println(sent);
radio.startListening();

}


