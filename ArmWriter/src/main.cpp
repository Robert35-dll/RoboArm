#include <Arduino.h>

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 Receiver(CE_PIN, CSN_PIN);
const byte R_ADDRESS[6] = "1RF24";

byte JTransfer[2];

void setup() {
    Serial.begin(9600);
    
    if (!Receiver.begin()) {
        Serial.println("[!]-< Receiver failed T_T");
        while (1);
    }
    Serial.println("[*]-< Receiver started ^~^");
    
    // Setting data channel
    Receiver.setDataRate(RF24_250KBPS);
    Receiver.setPALevel(RF24_PA_LOW);
    Receiver.setChannel(88);
    // Setting transmission's properties
    Receiver.setPayloadSize(sizeof(JTransfer));
    Receiver.setAutoAck(true);
    // Setting the receiver to retrieve data
    Receiver.openReadingPipe(0, R_ADDRESS);
    Receiver.startListening();

    Serial.println("[AW]-< Initialized");
}

void loop() {
    if (Receiver.available() > 0) {
        Receiver.read(&JTransfer, sizeof(JTransfer));
        Serial.print("[+]-< JYAxis: ");
        Serial.println(JTransfer[0]);
        Serial.print("[+]-< JButton: ");
        Serial.println(JTransfer[1]);
        Serial.println(" |");

        delay(1000);
    }
}