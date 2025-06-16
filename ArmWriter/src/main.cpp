#include <Arduino.h>

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 Receiver(CE_PIN, CSN_PIN);
const byte R_ADDRESS[6] = "1RF24";

byte JTransfer[2];

const int AxisAmount = 3;
// Arrays for ouput angles
uint16_t UpperAngles[AxisAmount];  // The angles of upper arm
uint16_t LowerAngles[AxisAmount];  // The angles of underarm

uint16_t JYAxisInput;
bool ButtonPressed;

int16_t TransArr[3];

void PrintData();

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
    Receiver.setPayloadSize(sizeof(TransArr));
    Receiver.setAutoAck(false);
    // Setting the receiver to retrieve data
    Receiver.openReadingPipe(0, R_ADDRESS);
    Receiver.startListening();

    Serial.println("[AW]-< Initialized");
}

void loop() {
    if (Receiver.available() > 0) {
        Receiver.read(&TransArr, sizeof(TransArr) * 2);

        LowerAngles[0] = TransArr[0];
        LowerAngles[1] = TransArr[1];
        LowerAngles[2] = TransArr[2];

        LowerAngles[3] = TransArr[3];
        LowerAngles[4] = TransArr[4];
        LowerAngles[5] = TransArr[5];

        JYAxisInput = TransArr[6];
        ButtonPressed = TransArr[7];

        PrintData();

        delay(1000);
    }
}

void PrintData() {
    Serial.print("[+]-< Upper MPU angles: ");
    Serial.print("X: ");
    Serial.print(UpperAngles[0]);
    Serial.print(" Y: ");
    Serial.print(UpperAngles[1]);
    Serial.print(" Z: ");
    Serial.println(UpperAngles[2]);

    Serial.print("[+]-< Lower MPU angles: ");
    Serial.print("X: ");
    Serial.print(LowerAngles[0]);
    Serial.print(" Y: ");
    Serial.print(LowerAngles[1]);
    Serial.print(" Z: ");
    Serial.println(LowerAngles[2]);
    Serial.println(" |");

    Serial.print("[+]-< Joystick Y-axis input: ");
    Serial.println(JYAxisInput);
    Serial.print("[+]-< Joystick button signal: ");
    Serial.println(ButtonPressed);
    Serial.println(" |");
}