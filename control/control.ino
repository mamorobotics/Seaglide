//https://projecthub.arduino.cc/tmekinyan/how-to-use-the-nrf24l01-module-with-arduino-813957

#include "SPI.h" 
#include "RF24.h" 
#include "nRF24L01.h" 
#include "Adafruit_MPRLS.h"
#define CE_PIN 9 
#define CSN_PIN 10 
#define INTERVAL_MS_TRANSMISSION 4000 

RF24 radio(CE_PIN, CSN_PIN); 

const byte address[6] = "00001";

int stepPin = 3;
int dirPin = 4;

int currentPos = 0;
int neutralPos = 0;
int floatPos = 200;
int sinkPos = -200;

float timeAtDepth = 0;

struct payload { 
	 String companyNumber; 
	 int time;
   float depth; 
}; 


Adafruit_MPRLS mpr = Adafruit_MPRLS(-1, -1);

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

  if (! mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found MPRLS sensor");
} 

void loop() 
{ 
	payload.companyNumber = '1234'; 
	payload.time = 123;
  payload.depth = mpr.readPressure()/10;

	radio.write(&payload, sizeof(payload)); 
	delay(INTERVAL_MS_TRANSMISSION); 

  if(timeAtDepth>=45){
    rotateToPos(floatPos);
  }
  else if(payload.depth/9.81 >= 2){
    rotateToPos(neutralPos);
    timeAtDepth++;
  }
  else if(payload.depth/9.81 <= 0.1){
    rotateToPos(neutralPos);
    sendPayloads();
    timeAtDepth = 0;
  }

  rotateToPos(400);
} 

void sendPayloads(){
  
}

void rotateToPos(int pos){
  int coeff = pos<currentPos ? -1 : 1;

  int steps = currentPos + (coeff * pos);

  currentPos = pos;

  if(steps < 0){
    digitalWrite(dirPin,HIGH);
  }else{
    digitalWrite(dirPin,LOW);
  }

  for(int x = 0; x < steps; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
  }
  delay(1000);
}