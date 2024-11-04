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

int stepPin = 2;
int dirPin = 3;

//stepper pos
int currentPos = 0;
int neutralPos = 0;
int floatPos = 200;
int sinkPos = -200;

//timing
float timeAtDepth = 0;
long startTime = millis();
long startDepthTime = 0;
bool firstDepth = true;

struct { 
	 String companyNumber; 
	 int time;
   float pressure; 
}payloads[50]; 


Adafruit_MPRLS mpr = Adafruit_MPRLS(-1, -1);

//payload payloads[50];
int lastStoredIndex = -1;

void setup() 
{ 
  Serial.begin(115200); 
  radio.begin(); 
  radio.setAutoAck(false);
  radio.setDataRate(RF24_1MBPS); 
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(sizeof(payloads[0])); 
  radio.openWritingPipe(address); 
  radio.stopListening(); 

  if (! mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found MPRLS sensor");

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
} 

void loop() 
{ 
  float pressure = mpr.readPressure()/10;

  if((millis()-startTime)%5000 >= 0 && (millis()-startTime)%5000 <= 200){
    lastStoredIndex += 1;
    payloads[lastStoredIndex].companyNumber = '1234'; 
	  payloads[lastStoredIndex].time = millis();
    payloads[lastStoredIndex].pressure = pressure;
  } 

  if(timeAtDepth>=45000){
    rotateToPos(floatPos);
  }
  else if(pressure/9.81 >= 2 && pressure/9.81 <= 3){
    if(firstDepth){startDepthTime = millis(); firstDepth = false;}
    else{timeAtDepth += (millis()-startDepthTime);}
    rotateToPos(neutralPos);
  }
  else if(pressure/9.81 <= 0.1){
    rotateToPos(neutralPos);
    sendPayloads();
    timeAtDepth = 0;
    firstDepth = true;
  }
} 

void sendPayloads(){
  for(int i=0; i<=lastStoredIndex; i++){
    radio.write(&payloads[i], sizeof(payloads[i])); 
	  delay(INTERVAL_MS_TRANSMISSION);
  }
}

void rotateToPos(int pos){
  if(currentPos == pos){return;}
  int coeff = pos<currentPos ? -1 : 1;

  if(currentPos<0 && coeff==1){
    currentPos *= -1;
  }

  int steps = currentPos + (coeff * pos);

  currentPos = pos;

  if(coeff == -1){
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
  Serial.println(currentPos);
  delay(1000);
}