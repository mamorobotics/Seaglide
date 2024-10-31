#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int MOSI = 11;
int MISO = 12;
int SCK = 13;
int CE = 7;
int CSN = 8;

RF24 radio(CE, CSN); // CE, CSN

const byte address[6] = "00001";

void setup() {
  radio.begin(); 
  radio.setAutoAck(false);
  radio.setDataRate(RF24_1MBPS); 
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(sizeof(payload)); 
  radio.openWritingPipe(address); 
  radio.stopListening(); 
}

void loop() {
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);

  // payload.companyNumber = '1234'; 
	// payload.time = 123;
  // payload.depth = mpr.readPressure()/10;

	// radio.write(&payload, sizeof(payload)); 
	// delay(INTERVAL_MS_TRANSMISSION); 
}