#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
extern SoftwareSerial BTSerial;
void trimiteMesaj(String text);
String citesteDePeTelefon();
String citesteDePeLaptop();