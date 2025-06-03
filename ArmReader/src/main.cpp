#include <Arduino.h>

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 Transmitter(CE_PIN, CSN_PIN);
const byte T_ADDRESS[6] = "1RF24";

// The joystick's vertical axis' pin
#define J_PIN_Y A0
// The joystick's toggle button pin
#define J_PIN_B A1

uint16_t JYAxisInput;
byte JYAxisTransfer;
bool ButtonPressed;

// The data transfer array
byte JTransfer[2];

void PrintData();

void setup() {
    Serial.begin(9600);

    // Joystick setup
    pinMode(J_PIN_Y, INPUT);
    pinMode(J_PIN_B, INPUT);

    // Transmitter setup
    if (!Transmitter.begin()) {
        Serial.println("[!]-< Transmitter failed T_T");
        while (1);
    }
    Serial.println("[*]-< Transmitter started ^~^");

    // Setting data channel
    Transmitter.setDataRate(RF24_250KBPS);
    Transmitter.setPALevel(RF24_PA_LOW);
    Transmitter.setChannel(88);
    // Setting transmission's properties
    Transmitter.setPayloadSize(sizeof(JTransfer));
    Transmitter.setAutoAck(true);
    // Setting an amount and delay to retry failed transmissions
    Transmitter.setRetries(5, 3);
    // Setting the transmitter to send data
    Transmitter.openWritingPipe(T_ADDRESS);
    Transmitter.stopListening();

    Serial.println("[AR]-< Initialized");
}

void loop() {
    JYAxisInput = analogRead(J_PIN_Y);
    ButtonPressed = analogRead(J_PIN_B) <= 100;

    JYAxisTransfer = map(JYAxisInput, 0, 1023, 0, 255);

    JTransfer[0] = JYAxisTransfer;
    JTransfer[1] = ButtonPressed;

    // Writing one (first) byte to the radio channel
    if (Transmitter.write(&JTransfer, sizeof(JTransfer), 0)) {
        PrintData();
    }

    delay(1000);
}

void PrintData() {
    Serial.print("[+]-< Joystick Y-axis input: ");
    Serial.println(JYAxisInput);
    Serial.print("[+]-< Joystick Y-axis transfer: ");
    Serial.println(JYAxisTransfer);
    Serial.println(" |");
    Serial.print("[+]-< Joystick button signal: ");
    Serial.println(ButtonPressed);
    Serial.println(" |");
}