#include <Arduino.h>

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 Receiver(CE_PIN, CSN_PIN);
const byte R_ADDRESS[11] = "TRF24_ADDR";

// The data transfer array
int16_t TransArr[7];

const int AxisAmount = 3;
// Arrays for ouput angles
int16_t UpperAngles[AxisAmount];    // The angles of upper arm
int16_t LowerAngles[AxisAmount];    // The angles of underarm

int16_t JYAxisInput;

// Servos' preparation
#include <Servo.h>

// Defining servos' pins
#define SX_PIN 3                    // Pin for shoulder's rotation at X-axis
#define SY_PIN 5                    // Pin for shoulder's rotation at Y-axis
#define SZ_PIN 6                    // Pin for shoulder's rotation at Z-axis
#define EL_PIN 9                    // Pin for elbow's bend
#define HA_PIN 10                   // Pin for hand's rotation along under arm

Servo SX;
Servo SY;
Servo SZ;
Servo EL;
Servo HA;

#define TIMEOUT 1000

void SetAngles();
void PrintData();

void setup() {
    Serial.begin(9600);
    
    pinMode(SX_PIN, OUTPUT);
    pinMode(SY_PIN, OUTPUT);
    pinMode(SZ_PIN, OUTPUT);
    pinMode(EL_PIN, OUTPUT);
    pinMode(HA_PIN, OUTPUT);

    SX.attach(SY_PIN);
    SY.attach(SX_PIN);
    SZ.attach(SZ_PIN);
    EL.attach(EL_PIN);
    HA.attach(HA_PIN);
    
    SX.write(90);
    SY.write(90);
    SZ.write(0);
    EL.write(0);
    HA.write(0);

    Serial.println("[*]-< Servos assigned to pins ^~^");
    
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
    Receiver.setAutoAck(true);
    // Setting the receiver to retrieve data
    Receiver.openReadingPipe(0, R_ADDRESS);
    Receiver.startListening();

    Serial.println("[AW]-< Initialized");
}

void loop() {
    if (Receiver.available() > 0) {
        Receiver.read(&TransArr, sizeof(TransArr) * 2);

        LowerAngles[0] = map(TransArr[0], -180, 180, 0, 180);
        LowerAngles[1] = map(TransArr[1], -180, 180, 0, 180);
        LowerAngles[2] = map(TransArr[2], -180, 180, 0, 180);

        UpperAngles[0] = map(TransArr[3], -180, 180, 0, 180);
        UpperAngles[1] = map(TransArr[4], -180, 180, 0, 180);
        UpperAngles[2] = map(TransArr[5], -180, 180, 0, 180);

        JYAxisInput = map(TransArr[6], 0, 1023, 0, 180);

        PrintData();

        SetAngles();

        delay(TIMEOUT);
    }
}

void SetAngles() {
    // Adjusting position of the shoulder 
    SX.write(UpperAngles[0]);
    SY.write(UpperAngles[1]);
    SZ.write(UpperAngles[2]);
    // Adjusting position of the elbow
    EL.write(LowerAngles[0]);
    // Adjusting position of the hand
    HA.write(JYAxisInput);
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
    Serial.println(" |");
}