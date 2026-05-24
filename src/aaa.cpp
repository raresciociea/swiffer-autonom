// #include <Arduino.h>
// #include <SoftwareSerial.h>

// #include "hc05_comm.hpp"
// extern SoftwareSerial BTSerial;
// // --- DEFINIRE PINI ---
// // Senzor Față (deja setat de tine pe A4/A5)
// const int trigFata = 2;  // Inainte era 3
// const int echoFata = 3;  // Inainte era 2

// // STÂNGA: PC2 (Trig) și PC3 (Echo)
// const int trigStanga = A2;
// const int echoStanga = A3;

// // DREAPTA: PC4 (Trig) și PC5 (Echo)
// const int trigDreapta = A4;
// const int echoDreapta = A5;

// // Senzor Podea (rămâne pe PC1)
// const int pinPodea = A1;

// unsigned long ultimaTrimitereBT = 0;
// const int intervalTrimitere = 1000;

// #define ENA 5  // PWM (PD5)
// #define IN1 4  // Digital (PD4)
// #define IN2 7  // Digital (PD7)

// // --- Motor Dreapta ---
// #define ENB 6   // PWM (PD6)
// #define IN3 10  // Digital (PB2)
// #define IN4 11  // Digital (PB3)

// // Prototip functie (acum primeste pini)
// int getDistanta(int trig, int echo);

// // --- VARIABILE PENTRU FSM (ZIG-ZAG) ---
// // --- VARIABILE PENTRU FSM (INTELIGENT) ---
// int stareRobot = 0;  // 0=STOP, 1=FATA, 2=INAPOI, 3=VIRAJ
// unsigned long timpInceputManevra = 0;
// const int timpInapoi = 400;  // 400 milisecunde pentru a da in spate
// int timpViraj = 600;         // 600 milisecunde pentru viraj
// const int vitezMica = 90;

// int vitezaStanga = 120;
// int vitezaDreapta = 150;

// char istoricDecizii[10];  // Va salva 'S' (Stanga) sau 'D' (Dreapta)
// int indexIstoric = 0;
// int contorBlocaje = 0;

// // O variabila sa tina minte decizia luata de senzori
// bool virajSpreStanga = false;

// void salveazaDecizie(char directie) {
//     istoricDecizii[indexIstoric] = directie;
//     indexIstoric++;

//     // Daca am umplut memoria, o luam de la capat
//     if (indexIstoric >= 10) {
//         indexIstoric = 0;
//     }
// }

// void opresteMotoare() {
//     digitalWrite(IN1, LOW);
//     digitalWrite(IN2, LOW);
//     digitalWrite(IN3, LOW);
//     digitalWrite(IN4, LOW);
//     analogWrite(ENA, 0);
//     analogWrite(ENB, 0);
// }

// void mergiInapoiIncet() {
//     digitalWrite(IN1, LOW);
//     digitalWrite(IN2, HIGH);
//     digitalWrite(IN3, LOW);
//     digitalWrite(IN4, HIGH);
//     analogWrite(ENA, vitezaStanga);   // Folosim viteza calibrata!
//     analogWrite(ENB, vitezaDreapta);  // Folosim viteza calibrata!
// }

// void mergiInainteIncet() {
//     digitalWrite(IN1, HIGH);
//     digitalWrite(IN2, LOW);
//     digitalWrite(IN3, HIGH);
//     digitalWrite(IN4, LOW);
//     analogWrite(ENA, vitezaStanga);
//     analogWrite(ENB, vitezaDreapta);
// }

// void rotesteStangaIncet() {
//     digitalWrite(IN1, LOW);
//     digitalWrite(IN2, HIGH);
//     digitalWrite(IN3, HIGH);
//     digitalWrite(IN4, LOW);
//     analogWrite(ENA, vitezaStanga);
//     analogWrite(ENB, vitezaDreapta);
// }

// void rotesteDreaptaIncet() {
//     digitalWrite(IN1, HIGH);
//     digitalWrite(IN2, LOW);
//     digitalWrite(IN3, LOW);
//     digitalWrite(IN4, HIGH);
//     analogWrite(ENA, vitezaStanga);
//     analogWrite(ENB, vitezaDreapta);
// }

// void setup() {
//     Serial.begin(9600);
//     BTSerial.begin(9600);

//     // Initializare pini ultrasonici
//     pinMode(trigFata, OUTPUT);
//     pinMode(echoFata, INPUT);
//     pinMode(trigStanga, OUTPUT);
//     pinMode(echoStanga, INPUT);
//     pinMode(trigDreapta, OUTPUT);
//     pinMode(echoDreapta, INPUT);

//     pinMode(ENA, OUTPUT);
//     pinMode(IN1, OUTPUT);
//     pinMode(IN2, OUTPUT);

//     pinMode(ENB, OUTPUT);
//     pinMode(IN3, OUTPUT);
//     pinMode(IN4, OUTPUT);

//     // Pinul de podea este intrare analogica
//     pinMode(pinPodea, INPUT);

//     delay(2000);
//     Serial.println("=== SISTEM MULTI-SENZOR ACTIVAT ===");
//     BTSerial.println("=== ROBOT ONLINE - TOȚI SENZORII ACTIVI ===");
// }

// // Functie scrisa in C Embedded pur pentru a afla cu cati Volti e alimentata
// // placa
// float citesteVccReal() {
//     // Setam registrii pentru a citi referinta interna de 1.1V
//     ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
//     delay(2);  // Asteptam 2 milisecunde sa se stabilizeze curentul

//     ADCSRA |= _BV(ADSC);               // Dam comanda de incepere a masuratorii
//     while (bit_is_set(ADCSRA, ADSC));  // Asteptam pana termina de masurat

//     uint8_t low = ADCL;   // Citim bitii de jos
//     uint8_t high = ADCH;  // Citim bitii de sus
//     long result = (high << 8) | low;

//     // Calculam voltajul real al placii (1125.3 = 1.1V * 1023)
//     float vccReal = 1125.3 / (float)result;
//     return vccReal;
// }
// int test_wheels = 0;
// void loop() {
//     // 1. Citim toti senzorii
//     int dFata = getDistanta(trigFata, echoFata);
//     delay(30);  // Pauza anti-ecou
//     int dStanga = getDistanta(trigStanga, echoStanga);
//     delay(30);
//     int dDreapta = getDistanta(trigDreapta, echoDreapta);

//     int valPodea = analogRead(pinPodea);

//     // 2. Afisam pe Laptop (Serial Monitor) pentru debug
//     /*
//     Serial.print("F:"); Serial.print(dFata);
//     Serial.print(" L:"); Serial.print(dStanga);
//     Serial.print(" R:"); Serial.print(dDreapta);
//     Serial.print(" P:"); Serial.println(valPodea);
//     */

//     // 3. Trimitem pe Telefon datele grupate (la 1 secunda)
//     // 3. Trimitem pe Telefon datele grupate (la 1 secunda)
//     if (millis() - ultimaTrimitereBT > intervalTrimitere) {
//         // --- CITIRE BATERIE INTELIGENTA ---
//         // 1. Aflam cu ce tensiune e alimentata placa in momentul asta
//         float riglaCurenta = citesteVccReal();

//         // 2. Citim valoarea bruta de la rezistente (0 - 1023)
//         int valoareBruta = analogRead(A0);

//         // 3. Calculam folosind voltajul REAL, nu unul presupus
//         float tensiuneBaterie = valoareBruta * (riglaCurenta / 1023.0) * 2.0;

//         // --- CONSTRUIRE MESAJ TELEMETRIE ---
//         String telemetrie = "F:" + String(dFata) + " L:" + String(dStanga) +
//                             " R:" + String(dDreapta) +
//                             " P:" + String(valPodea) +
//                             " Bat:" + String(tensiuneBaterie, 2) + "V";

//         trimiteMesaj(telemetrie);
//         ultimaTrimitereBT = millis();
//     }

//     // 4. Logica de Bluetooth (Comenzi)
//     String mesajTlf = citesteDePeTelefon();
//     if (mesajTlf.length() > 0) {
//         Serial.print("[BT]: ");
//         Serial.println(mesajTlf);
//         if (mesajTlf == "ping") trimiteMesaj("PONG!");

//         if (mesajTlf.length() > 0) {
//             Serial.print("[BT]: ");
//             Serial.println(mesajTlf);

//             if (mesajTlf == "start") {
//                 stareRobot = 1;  // Trecem in starea MERGE_FATA
//                 BTSerial.println("Incep modul Zig-Zag!");
//             } else if (mesajTlf == "stop") {
//                 stareRobot = 0;  // Trecem in starea STOP
//                 opresteMotoare();
//                 BTSerial.println("Sistem Oprit.");
//             }
//         }

//         if (mesajTlf == "test_wheels") {
//             // test_wheels = 1 - test_wheels;  // Toggle intre 0 si 1
//             // if (test_wheels) {
//             // Testare motoare: inainte 2 secunde, inapoi 2 secunde
//             digitalWrite(IN1, HIGH);
//             digitalWrite(IN2, LOW);
//             digitalWrite(IN3, HIGH);
//             digitalWrite(IN4, LOW);
//             analogWrite(ENA, 100);  // Viteza medie
//             analogWrite(ENB, 100);
//             delay(1000);

//             digitalWrite(IN1, LOW);
//             digitalWrite(IN2, HIGH);
//             digitalWrite(IN3, LOW);
//             digitalWrite(IN4, HIGH);
//             delay(1000);

//             // Oprim motoarele
//             digitalWrite(IN1, LOW);
//             digitalWrite(IN2, LOW);
//             digitalWrite(IN3, LOW);
//             digitalWrite(IN4, LOW);
//             //  } else {
//             // Oprim motoarele
//             digitalWrite(IN1, LOW);
//             digitalWrite(IN2, LOW);
//             digitalWrite(IN3, LOW);
//             digitalWrite(IN4, LOW);
//             // }
//         }
//     }

//     // 5. Logica de Laptop -> Telefon
//     String mesajLpt = citesteDePeLaptop();
//     if (mesajLpt.length() > 0) {
//         trimiteMesaj(mesajLpt);
//     }

//     delay(50);

//     // --- FSM: LOGICA INTELIGENTA DE EVITARE ---
//     if (stareRobot == 1) {
//         // STAREA 1: MERGEM INAINTE
//         mergiInainteIncet();

//         if (dFata < 30) {
//             stareRobot = 2;                 // Trecem in modul de dat inapoi
//             timpInceputManevra = millis();  // Pornim cronometrul manevrei
//         }
//     } else if (stareRobot == 2) {
//         // STAREA 2: DAM INAPOI CA SA FACEM LOC
//         mergiInapoiIncet();

//         // Dupa ce a trecut timpul de dat inapoi, trecem la viraj
//         if (millis() - timpInceputManevra > timpInapoi) {
//             stareRobot = 3;
//             timpInceputManevra = millis();  // Resetam cronometrul pentru viraj

//             // Aici e decizia inteligenta: Unde avem mai mult loc?
//             if (dStanga > dDreapta) {
//                 virajSpreStanga = true;
//             } else {
//                 virajSpreStanga = false;
//             }
//         }
//     } else if (stareRobot == 3) {
//         // STAREA 3: VIRAM PE LOC (spre zona cea mai libera)
//         if (virajSpreStanga) {
//             rotesteStangaIncet();
//         } else {
//             rotesteDreaptaIncet();
//         }

//         // Dupa ce s-a rotit cat trebuie, revine la mers inainte
//         if (millis() - timpInceputManevra > timpViraj) {
//             stareRobot = 1;
//         }
//     }

//     // ... codul tau existent ...
//     if (dStanga > dDreapta) {
//         virajSpreStanga = true;
//         salveazaDecizie('S');
//     } else {
//         virajSpreStanga = false;
//         salveazaDecizie('D');
//     }

//     // Verificam daca suntem prinsi intr-o capcana (ex: S, D, S, D)
//     // Ne uitam in urma la ultimele 4 decizii
//     int sCount = 0;
//     int dCount = 0;
//     for (int i = 0; i < 10; i++) {
//         if (istoricDecizii[i] == 'S') sCount++;
//         if (istoricDecizii[i] == 'D') dCount++;
//     }

//     // Daca are un numar aproape egal de stanga/dreapta recente si senzorii
//     // arata distante mici
//     if (sCount > 3 && dCount > 3 && dFata < 30) {
//         contorBlocaje++;
//         if (contorBlocaje > 2) {
//             // PANICA! Suntem intr-un colt!
//             // Fortam un viraj extrem de 180 de grade
//             timpViraj = 1200;        // Dublam timpul de viraj pentru a intoarce
//                                      // complet spatele
//             virajSpreStanga = true;  // Rotim mereu spre stanga ca sa iesim
//             contorBlocaje = 0;       // Resetam contorul

//             // Curatam memoria pentru noul drum
//             for (int k = 0; k < 10; k++) istoricDecizii[k] = ' ';
//         }
//     } else {
//         timpViraj = 600;  // Timpul normal de evitare
//     }
// }

// // --- FUNCTIE UNIVERSALA DISTANTA ---
// int getDistanta(int trig, int echo) {
//     digitalWrite(trig, LOW);
//     delayMicroseconds(2);
//     digitalWrite(trig, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(trig, LOW);

//     // pulseIn returneaza durata in microsecunde
//     long durata = pulseIn(echo, HIGH, 25000);  // Timeout la 25ms

//     if (durata == 0) return 400;  // Daca nu primim ecou, consideram drum liber

//     int d = durata * 0.034 / 2;
//     if (d > 400) return 400;
//     return d;
// }