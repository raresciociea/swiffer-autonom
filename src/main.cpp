#include <Arduino.h>
#include <SoftwareSerial.h>

#include "hc05_comm.hpp"

extern SoftwareSerial BTSerial;

// --- DEFINIRE PINI ---
const int trigFata = 2;
const int echoFata = 3;
const int trigStanga = A2;
const int echoStanga = A3;
const int trigDreapta = A4;
const int echoDreapta = A5;
const int pinPodea = A1;

unsigned long ultimaTrimitereBT = 0;
const int intervalTrimitere = 1000;

#define ENA 5
#define IN1 4
#define IN2 7
#define ENB 6
#define IN3 10
#define IN4 11

int getDistanta(int trig, int echo);

// --- VARIABILE DE STARE ȘI VITEZĂ ---
int stareRobot = 0;
unsigned long timpInceputManevra = 0;
const int timpInapoi = 400;
int timpViraj = 600;

const int vitezMica = 90;
int vitezaStanga = 255;
int vitezaDreapta = 255;
int vitezaViraj = 255;

char istoricDecizii[10];
int indexIstoric = 0;
unsigned long ultimulTimpObstacol = 0;
int contorLovituriRapide = 0;
bool virajSpreStanga = false;

void salveazaDecizie(char directie) {
    istoricDecizii[indexIstoric] = directie;
    indexIstoric++;
    if (indexIstoric >= 10) {
        indexIstoric = 0;
    }
}

void opresteMotoare() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, 255);
    analogWrite(ENB, 255);
    delay(50);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void pornireInForta() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, 255);
    analogWrite(ENB, 255);
    delay(50);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 230);
    analogWrite(ENB, 255);
    delay(250);
}

void mergiInainteIncet() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, vitezaStanga);
    analogWrite(ENB, vitezaDreapta);
}

void mergiInapoiIncet() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, vitezaViraj);
    analogWrite(ENB, vitezaViraj);
}

void rotesteStangaIncet() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, vitezaViraj);
    analogWrite(ENB, vitezaViraj);
}

void rotesteDreaptaIncet() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, vitezaViraj);
    analogWrite(ENB, vitezaViraj);
}

void setup() {
    Serial.begin(9600);
    BTSerial.begin(9600);

    randomSeed(analogRead(A6));

    pinMode(trigFata, OUTPUT);
    pinMode(echoFata, INPUT);
    pinMode(trigStanga, OUTPUT);
    pinMode(echoStanga, INPUT);
    pinMode(trigDreapta, OUTPUT);
    pinMode(echoDreapta, INPUT);
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(pinPodea, INPUT);

    for (int i = 0; i < 10; i++) istoricDecizii[i] = 'X';

    delay(2000);
    Serial.println("=== SISTEM MULTI-SENZOR ACTIVAT ===");
    BTSerial.println("=== ROBOT ONLINE - TOȚI SENZORII ACTIVI ===");
}

float citesteVccReal() {
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(3);
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC));
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC));
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    long result = (high << 8) | low;
    return 1125.3 / (float)result;
}

void loop() {
    int dFata = getDistanta(trigFata, echoFata);
    int dStanga = getDistanta(trigStanga, echoStanga);
    int dDreapta = getDistanta(trigDreapta, echoDreapta);
    int valPodea = analogRead(pinPodea);

    if (millis() - ultimaTrimitereBT > intervalTrimitere) {
        float riglaCurenta = citesteVccReal();
        analogRead(A0);
        delay(3);
        int valoareBruta = analogRead(A0);
        float tensiuneBaterie = valoareBruta * (riglaCurenta / 1023.0) * 2.0;

        String telemetrie = "F:" + String(dFata) + " L:" + String(dStanga) +
                            " R:" + String(dDreapta) +
                            " P:" + String(valPodea) +
                            " Bat:" + String(tensiuneBaterie, 2) + "V";

        trimiteMesaj(telemetrie);
        Serial.println(telemetrie);
        ultimaTrimitereBT = millis();
    }

    String mesajTlf = citesteDePeTelefon();
    mesajTlf.trim();
    if (mesajTlf.length() > 0) {
        if (mesajTlf == "ping") trimiteMesaj("PONG!");
        if (mesajTlf == "start") {
            pornireInForta();
            stareRobot = 1;
            BTSerial.println("Incep modul de acoperire avansata!");
            Serial.println("Robot PORNIT!");
        } else if (mesajTlf == "stop") {
            stareRobot = 0;
            opresteMotoare();
            BTSerial.println("Sistem Oprit.");
            Serial.println("Robot OPRIT!");
        }
    }

    String mesajLpt = citesteDePeLaptop();
    if (mesajLpt.length() > 0) {
        trimiteMesaj(mesajLpt);
    }

    bool pericolPodea = (valPodea < 300);

    if (stareRobot == 1) {
        mergiInainteIncet();
        if (dFata < 30 || pericolPodea) {
            stareRobot = 2;
            timpInceputManevra = millis();

            if (millis() - ultimulTimpObstacol < 5000) {
                contorLovituriRapide++;
            } else {
                contorLovituriRapide = 0;
            }
            ultimulTimpObstacol = millis();
        }
    } else if (stareRobot == 2) {
        mergiInapoiIncet();
        if (millis() - timpInceputManevra > timpInapoi) {
            stareRobot = 3;
            timpInceputManevra = millis();

            int sCount = 0;
            int dCount = 0;
            for (int i = 0; i < 10; i++) {
                if (istoricDecizii[i] == 'S') sCount++;
                if (istoricDecizii[i] == 'D') dCount++;
            }

            if (contorLovituriRapide > 3 || sCount >= 8 || dCount >= 8) {
                virajSpreStanga = random(0, 2);
                timpViraj = random(1200, 1800);
                contorLovituriRapide = 0;
                for (int i = 0; i < 10; i++) istoricDecizii[i] = 'X';
            } else {
                if (dStanga > dDreapta + 10) {
                    virajSpreStanga = true;
                } else if (dDreapta > dStanga + 10) {
                    virajSpreStanga = false;
                } else {
                    virajSpreStanga = random(0, 2);
                }
                timpViraj = random(400, 900);
            }
            salveazaDecizie(virajSpreStanga ? 'S' : 'D');
        }
    } else if (stareRobot == 3) {
        if (virajSpreStanga) {
            rotesteStangaIncet();
        } else {
            rotesteDreaptaIncet();
        }

        if (millis() - timpInceputManevra > timpViraj) {
            pornireInForta();
            stareRobot = 1;
        }
    }
}

int getDistanta(int trig, int echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    long durata = pulseIn(echo, HIGH, 3500);

    if (durata == 0) return 400;

    int d = durata * 0.034 / 2;
    if (d > 400 || d < 0) return 400;

    return d;
}