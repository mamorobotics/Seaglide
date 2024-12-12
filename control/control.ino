#include <SPI.h>
#include <RH_RF95.h>
#include "Adafruit_MPRLS.h"

// Define pins for the RFM96W
#define RFM95_CS 10   // Chip select
#define RFM95_RST 9   // Reset
#define RFM95_INT 2   // Interrupt

// Define frequency (433 MHz)
#define RF95_FREQ 433.0

// Create an instance of the driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int stepPin = 3;
int dirPin = 4;

int currentPos = 0;
int neutralPos = 0;
int floatPos = 1000;
int sinkPos = -1000;

int count = 0;

long startTime;
long startDepthTime = 0;
long timeAtDepth = 0;
bool firstControllerData = true;
bool firstDepth = true;

struct {  
	 float time;
   float pressure; 
}payloads[120]; 


Adafruit_MPRLS mpr = Adafruit_MPRLS(-1, -1);

int lastStoredIndex = -1;

void setup() 
{ 
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // Wait for Serial monitor to connect
  }
  Serial.println("LoRa RFM96W Test");

  // Reset the module
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Initialize the radio
  if (!rf95.init()) {
    Serial.println("RFM96W LoRa radio init failed");
    while (1);
  }
  Serial.println("RFM96W LoRa radio init OK!");

  // Set frequency
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("Failed to set frequency");
    while (1);
  }
  Serial.print("Frequency set to: ");
  Serial.println(RF95_FREQ);

  // Set transmitter power
  rf95.setTxPower(13, false); // 13 dBm

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(13, OUTPUT);

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
  float pressure = mpr.readPressure()/10;

  if((millis()-startTime)%5000 >= 0 && (millis()-startTime)%5000 <= 200){
    lastStoredIndex += 1;
	  payloads[lastStoredIndex].time = millis()/60000.0;
    payloads[lastStoredIndex].pressure = pressure;
  } 

  if(timeAtDepth>=45000){
    rotateToPos(floatPos);

    if(pressure/9.81 <= 0.1){
      rotateToPos(neutralPos);
      sendPayloads();
      timeAtDepth = 0;
      firstDepth = true;
      lastStoredIndex = 0;
    }
  } else{
    int pos = getPos(pressure/9.81);
    rotateToPos(pos);
    if(pressure/9.81 >= 2 && pressure/9.81 <= 3){
      if(firstDepth){
        startDepthTime = millis(); 
        firstDepth = false;
      }else{
        timeAtDepth += (millis()-startDepthTime);
      }
    }else
      timeAtDepth = 0;
      firstDepth = true;
      lastStoredIndex = 0;
    }
  }
  // else if(pressure/9.81 >= 2 && pressure/9.81 <= 3){
  //   if(firstDepth){
  //     startDepthTime = millis(); 
  //     firstDepth = false;
  //   }
  //   else{
  //     timeAtDepth += (millis()-startDepthTime);
  //   }
  //   rotateToPos(neutralPos);
  // } else if(pressure/9.81 <= 0.1){
  //   rotateToPos(neutralPos);
  //   sendPayloads();
  //   timeAtDepth = 0;
  //   firstDepth = true;
  //   lastStoredIndex = 0;
  // } else if(pressure/9.81 > 3){
  //   rotateToPos(floatPos/2);
  //   timeAtDepth = 0;
  //   firstDepth = true;
  //   lastStoredIndex = 0;
  // }else if(pressure/9.81 > 1 && timeAtDepth <= 45000){
  //   rotateToPos(sinkPos/2);
  //   timeAtDepth = 0;
  //   firstDepth = true;
  //   lastStoredIndex = 0;
  // }
} 

void sendPayloads(){
  for(int i=0; i<=lastStoredIndex; i++){
    String dataString = "RN01!";
    dataString = dataString + payloads[i].time;
    dataString = dataString + "!";
    dataString = dataString + payloads[i].pressure;

    uint8_t data[dataString.length() + 1];
    memcpy(data, dataString.c_str(), dataString.length() + 1);

    rf95.send(data, sizeof(data)); 
	  rf95.waitPacketSent();
    delay(500);
  }

  memset(payloads, 0, sizeof(payloads));
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
int getPos(int depth){
  int maxVal = 1000;
  int pow = 7;

  int coeff = ((-1)/Math.pow((-1),pow))*maxVal;
  int pos = Math.pow((depth-2.5), pow);
  return coeff*pos;
}