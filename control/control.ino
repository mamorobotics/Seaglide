//https://projecthub.arduino.cc/tmekinyan/how-to-use-the-nrf24l01-module-with-arduino-813957

#include "SPI.h" 
#include "RF24.h" 
#include "nRF24L01.h" 
#define CE_PIN 9 
#define CSN_PIN 10 
#define INTERVAL_MS_TRANSMISSION 4000 

RF24 radio(CE_PIN, CSN_PIN); 

const byte address[6] = "00001";

//NRF24L01 buffer limit is 32 bytes (max struct size) 

struct payload { 
	 char[4] companyNumber; 
	 int time;
   int depth; 
}; 

payload payload; 

void setup() 
{ 
	 Serial.begin(115200); 
	 radio.begin(); 
	 radio.setAutoAck(false);
	 radio.setDataRate(RF24_1MBPS); 
	 radio.setPALevel(RF24_PA_MAX);
	 radio.setPayloadSize(sizeof(payload)); 
	 radio.openWritingPipe(address); 
	 radio.stopListening(); 
} 

void loop() 
{ 
	 payload.companyNumber = '1234'; 
	 payload.time = 123;
   payload.depth = 123;
	 radio.write(&payload, sizeof(payload)); 
	 delay(INTERVAL_MS_TRANSMISSION); 
} 
