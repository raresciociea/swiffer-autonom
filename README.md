# Robot Autonom pentru Curatenie (Swiffer Robot)

Proiect de robot autonom dezvoltat pe platforma **ATmega328P**, optimizat pentru navigare inteligenta, evadare din colturi si acoperire eficienta a spatiului fara blocaje repetitive.

## Caracteristici Tehnice & Algoritmi
* **Arhitectura 100% Non-blocanta:** Implementare bazata pe soft-timere (`millis()`), fara utilizarea functiei blocante `delay()`.
* **Masina de Stari Finita (FSM):** Gestionare curata a comportamentului prin stari dedicate (`Mers Inainte`, `Mers Inapoi`, `Rotire`, `Stop`).
* **Acoperire Organica (Memorie pe 10 Generatii):** Robotul salveaza ultimele 10 decizii intr-un vector circular. Foloseste unghiuri si timpi de viraj aleatorii (`random(400, 900)ms`) pentru a evita buclele infinite. La blocaje repetitive, declanseaza o manevra lunga de evadare (U-Turn).
* **Telemetrie Bluetooth in Timp Real:** Optimizare hardware prin scaderea timeout-ului `pulseIn()` la 3500µs pentru senzorii ultrasonici, eliminand complet lag-ul de pe modulul HC-05.
* **Citiri ADC Stabile (Fara Fluctutatii):** Tehnica *"Double Dummy Read"* pentru stabilizarea multiplexorului ADC intre senzorul de podea TCRT5000 si divizorul de tensiune al bateriei (raporteaza ~8.4V stabil).

## Componente Hardware
* **Creier:** Microcontroler ATmega328P
* **Senzori Obstacole:** 3x Senzori Ultrasonici (Fata, Stanga, Dreapta)
* **Senzor Prapastie/Margine:** 1x Senzor Infrarosu TCRT5000 (orientat spre podea)
* **Actuatori:** 2x Motoare DC conduse de o punte H **L298N**
* **Comunicare:** Modul Bluetooth **HC-05**
* **Alimentare:** Pachet baterii LiPo (2S)

## Structura Proiectului (PlatformIO)
* `src/main.cpp` — Masina de stari, algoritmul decizional, citirea senzorilor si controlul PWM al motoarelor.
* `include/hc05_comm.hpp` — Header custom dedicat pentru parsarea si curatarea bufferului serial Bluetooth.
* `platformio.ini` — Fisierul de configurare al mediului de dezvoltare.

## Ghid de Compilare si Incarcare
1. Instaleaza **VS Code** si extensia **PlatformIO IDE**.
2. Cloneaza/Deschide folderul acestui proiect in VS Code.
3. Conecteaza placuta prin cablu USB.
4. Apasa pe butonul **PlatformIO: Upload** (sageata din bara de jos) pentru compilare si scriere automata pe cip.
