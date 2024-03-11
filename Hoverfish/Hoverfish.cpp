#pragma once

#include <iostream>
#include <RF24.h>
#include <string>

int main(){
    std::string message = "";
    std::string num = "";
    double depth;
    double pressure;
    int time;

    //         CE, CSN
    RF24 radio(15, 0);

    if (!radio.begin()) {
        cout << "radio hardware is not responding!!" << endl;
        return 0; // quit now
    }
    radio.setPALevel(RF24_PA_MAX);
    radio.openWritingPipe("YEET BOI LAN");


    message = num + "!" + std::string(time) + "!" + std::string(pressure) + "!" + std::string(depth);

    radio.setPayloadSize(sizeof(message));
    radio.write(&message, sizeof(message));
}