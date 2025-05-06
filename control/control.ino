#include <SPI.h>
#include <Wire.h>
#include <RH_RF95.h>
#include "Adafruit_MPRLS.h"

// Create an instance of the driver
RH_RF95 rf95(10, 2);

int currentPos = 0;
int neutralPos = 0;
int floatPos = 0;

int count = 0;

long startTime;
long startDepthTime = 0;
long timeAtDepth = 0;
bool firstControllerData = true;
bool firstDepth = true;

struct payload{  
	 int time;
   int pressure; 
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
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  delay(10);
  digitalWrite(9, LOW);
  delay(10);
  digitalWrite(9, HIGH);
  delay(10);

  Wire.begin();

  // Initialize the radio
  if (!rf95.init()) {
    Serial.println("RFM96W LoRa radio init failed");
    while (1);
  }
  Serial.println("RFM96W LoRa radio init OK!");

  // Set frequency
  if (!rf95.setFrequency(433.0)) {
    Serial.println("Failed to set frequency");
    while (1);
  }
  Serial.print("Frequency set to: ");
  Serial.println(433.0);

  // Set transmitter power
  rf95.setTxPower(13, false); // 13 dBm

  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(13, OUTPUT);

  if (!mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found MPRLS sensor");
  delay(100);
} 

void loop() 
{ 
  int pressure = int(mpr.readPressure() * 10);
  Serial.println(pressure);
  
  if((millis()-startTime)%5000 >= 0 && (millis()-startTime)%5000 <= 200){
    lastStoredIndex += 1;
	  payloads[lastStoredIndex].time = int(millis()/600.0);
    payloads[lastStoredIndex].pressure = pressure;

    if(pressure/9.81 >= 2 && pressure/9.81 <= 3){
      if(firstDepth){
        startDepthTime = millis(); 
        firstDepth = false;
      }
    }
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
  } 
  else{
    int pos = getPos(pressure/9.81);
    rotateToPos(pos);
    if(pressure/9.81 >= 2 && pressure/9.81 <= 3 && !firstDepth){
        timeAtDepth += (millis()-startDepthTime);
    }else{
      timeAtDepth = 0;
      firstDepth = true;
      lastStoredIndex = 0;
    }
  }

  //end skibidi

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
  Serial.println("sending");
  for(int i=0; i<=lastStoredIndex; i++){
    String dataString = "RN01!";
    payload p = payloads[i];
    Serial.println(p.pressure);
    //dataString = dataString + str;
    // dataString = dataString + "!";
    // dataString = dataString + payloads[i].pressure;

    // uint8_t data[dataString.length() + 1];
    // memcpy(data, dataString.c_str(), dataString.length() + 1);

    // rf95.send(data, sizeof(data)); 
	  // rf95.waitPacketSent();
    // delay(500);
  }

  //memset(payloads, 0, sizeof(payloads));
}

void rotateToPos(int pos){
  pos = constrain(pos, 50, 3200);
  if(currentPos == pos){return;}
  int coeff = pos<currentPos ? -1 : 1;

  int steps = abs(pos - currentPos);

  currentPos = pos;

  if(coeff == -1){
    digitalWrite(4,HIGH);
  }else{
    digitalWrite(4,LOW);
  }

  for(int x = 0; x < steps; x++) {
    digitalWrite(3,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(3,LOW); 
    delayMicroseconds(500); 
  }
  delay(500);
}

int getPos(int depth){
  int power = 7;

  int pos = -1 * pow((depth-2.5), power) + 1575;
  return constrain(pos, 50, 3200);
}