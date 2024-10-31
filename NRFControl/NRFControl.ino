#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int CE = 7;
int CSN = 8;

RF24 radio(CE, CSN); // CE, CSN

const byte address[6] = "00001";

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening(); 
}

void loop() {
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);
}