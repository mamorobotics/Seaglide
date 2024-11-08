#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);

uint8_t pipeNumber;
uint8_t payloadSize;
uint8_t status;

const uint64_t pipeNum = 0x7878787878LL;

void setup() {

  Serial.begin(9600);
  if(!radio.begin()){
    Serial.println("Radio Begin Failed :(");
  }
  radio.setChannel(100);
  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(15, 15);
  radio.setDataRate (RF24_1MBPS);

  radio.openWritingPipe(pipeNum);
  radio.stopListening();

  radio.enableDynamicPayloads();
  //radio.disableAckPayload();
}

char text[] = "Hello world";

void loop() {
  if(radio.isFifo(true)==1){
    if (radio.write(&text, sizeof(text))) {
      Serial.println("Delivered " + String(sizeof(text)) + " byte");
    }
    else {
      Serial.println("Data not delivered");
    }
  }else{
    Serial.println("FIFO full");
  }

  // if (!radio.isChipConnected()) {
  //   Serial.println("NRF24L01+ module not connected!");
  // } else {
  //     Serial.println("NRF24L01+ module is connected.");
  // }
  delay(1000);

}