#include <Arduino.h>

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 Transmitter(CE_PIN, CSN_PIN);
const byte T_ADDRESS[6] = "1RF24";

// MPU6050's libraries
#include <MPU6050_tockn.h>
#include <Wire.h>

// The MPUs' mute pins
#define UPPER_MPU_ADO 2
#define LOWER_MPU_ADO 3

const int AnglesAmount = 3;
// Arrays for MPUs' values
int UpperMPUAngles[AnglesAmount];
int LowerMPUAngles[AnglesAmount];

// The MPU module Arduino's currently listening to
MPU6050 ActiveMPU(Wire);

// The joystick's vertical axis' pin
#define J_PIN_Y A0
// The joystick's toggle button pin
#define J_PIN_B A1

uint16_t JYAxisInput;
bool ButtonPressed;

// The data transfer array
byte JTransfer[2];

void GetAngles(int, int , int (*)[3]);
void PrintData();

void setup() {
    Serial.begin(115200);

    Serial.println("[AR]-< Started initialization");
    pinMode(UPPER_MPU_ADO, OUTPUT);
    pinMode(LOWER_MPU_ADO, OUTPUT);
    // Muting one of the MPUs for initializing
    digitalWrite(UPPER_MPU_ADO, HIGH);
    digitalWrite(LOWER_MPU_ADO, LOW);

    // Initializing and calibrating the active MPU module 
    Serial.println("[*]-< Initializing MPU monitoring object");
    Wire.begin();
    ActiveMPU.begin();
    ActiveMPU.calcGyroOffsets();

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
    // Reading angles of the arm
    GetAngles(LOWER_MPU_ADO, UPPER_MPU_ADO, &UpperMPUAngles);
    GetAngles(UPPER_MPU_ADO, LOWER_MPU_ADO, &LowerMPUAngles);
    // TODO: Packing them into the transfer array

    JYAxisInput = analogRead(J_PIN_Y);
    ButtonPressed = analogRead(J_PIN_B) <= 100;

    JTransfer[0] = JYAxisInput;
    JTransfer[1] = ButtonPressed;

    // Writing one (first) byte to the radio channel
    if (Transmitter.write(&JTransfer, sizeof(JTransfer), 0)) {
        PrintData();
    }

    delay(1000);
}

void GetAngles(int mutePin, int unMutePin, int (*angleArrPtr)[3]) {
    // Muting the given (other) MPU module
    digitalWrite(mutePin, HIGH);
    digitalWrite(unMutePin, LOW);

    delay(1000);

    // Updating the data of currently active one
    ActiveMPU.update();

    int x = ActiveMPU.getGyroAngleX();
    int y = ActiveMPU.getGyroAngleY();
    int z = ActiveMPU.getGyroAngleZ();

    (*angleArrPtr)[0] = x;
    (*angleArrPtr)[1] = y;
    (*angleArrPtr)[2] = z;

    Serial.print(" |-< X: ");
    Serial.print(x);
    Serial.print(" Y: ");
    Serial.print(y);
    Serial.print(" Z: ");
    Serial.println(z);
    Serial.println(" |");

    // Activating the muted MPU back 
    digitalWrite(unMutePin, HIGH);
}

void PrintData() {
    Serial.print("[+]-< Upper MPU angles: ");
    Serial.print("X: ");
    Serial.print(UpperMPUAngles[0]);
    Serial.print(" Y: ");
    Serial.print(UpperMPUAngles[1]);
    Serial.print(" Z: ");
    Serial.println(UpperMPUAngles[2]);
    Serial.println(" |");

    Serial.print("[+]-< Lower MPU angles: ");
    Serial.print("X: ");
    Serial.print(LowerMPUAngles[0]);
    Serial.print(" Y: ");
    Serial.print(LowerMPUAngles[1]);
    Serial.print(" Z: ");
    Serial.println(LowerMPUAngles[2]);
    Serial.println(" |");

    Serial.print("[+]-< Joystick Y-axis input: ");
    Serial.println(JYAxisInput);
    Serial.print("[+]-< Joystick button signal: ");
    Serial.println(ButtonPressed);
    Serial.println(" |");
}