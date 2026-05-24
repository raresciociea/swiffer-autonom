#include "hc05_comm.hpp"

SoftwareSerial BTSerial(8, 9);

void trimiteMesaj(String text) { BTSerial.println(text); }


String citesteDePeLaptop() {
    String temp = "";
    if (Serial.available() > 0) {
        while (Serial.available() > 0) {
            char c = Serial.read();
            Serial.print(c);  

            if (c == '\n' || c == '\r') break;
            temp += c;
            delay(5);
        }
    }
    temp.trim();
    return temp;
}

String citesteDePeTelefon() {
    String temp = "";
    while (BTSerial.available() > 0) {
        char c = BTSerial.read();
        temp += c;
        delay(5);

    }
    temp.trim();
    return temp;
}