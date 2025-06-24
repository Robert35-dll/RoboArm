#include <Arduino.h>

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 Receiver(CE_PIN, CSN_PIN);
const byte R_ADDRESS[11] = "TRF24_ADDR";

// The data transfer array
int16_t TransArr[8];

const int AxisAmount = 3;
// Arrays for ouput angles
int16_t UpperAngles[AxisAmount];    // The angles of upper arm
int16_t LowerAngles[AxisAmount];    // The angles of underarm

int16_t JYAxisInput;
bool ButtonPressed;

// Servos' preparation
#include <Servo.h>

// Defining servos' pins
#define SX_PIN 0                    // Pin for shoulder's rotation at X-axis
#define SY_PIN 1                    // Pin for shoulder's rotation at Y-axis
#define SZ_PIN 2                    // Pin for shoulder's rotation at Z-axis
#define EL_PIN 3                    // Pin for elbow's bend
#define HA_PIN 4                    // Pin for hand's rotation along under arm
#define F1_PIN 5                    // Pin for the first finger 
#define F2_PIN 6                    // Pin for the second finger

const int ServosAmount = 7;
const int ServoPins[ServosAmount] = {
    SX_PIN,
    SY_PIN,
    SZ_PIN,
    EL_PIN,
    HA_PIN,
    F1_PIN,
    F2_PIN
};
Servo Servos[ServosAmount];

const int StartAngle = 90;

#define TIMEOUT 10

void SetAngles();
void PrintData();

void setup() {
    Serial.begin(9600);
    
    for (int i = 0; i < ServosAmount; i++) {
        Servos[i].attach(ServoPins[i]);
        Servos[i].write(StartAngle);
    }
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

        LowerAngles[0] = TransArr[0];
        LowerAngles[1] = TransArr[1];
        LowerAngles[2] = TransArr[2];

        UpperAngles[0] = TransArr[3];
        UpperAngles[1] = TransArr[4];
        UpperAngles[2] = TransArr[5];

        JYAxisInput = TransArr[6];
        ButtonPressed = TransArr[7];

        PrintData();

        SetAngles();

        delay(TIMEOUT);
    }
}

void SetAngles() {
    // Adjusting position of the shoulder 
    for (int i = 0; i < 3; i++) {
        Servos[i].write(StartAngle + UpperAngles[i]);
    }
    // Adjusting position of the elbow
    Servos[3].write(StartAngle + LowerAngles[3]);
    // Adjusting position of the hand
    Servos[4].write(map(JYAxisInput, 0, 1023, 0, 180));
    // Some dummy implementation of holding :3
    Servos[5].write(map(ButtonPressed, 0, 1, 0, 45));
    Servos[6].write(map(ButtonPressed, 0, 1, 45, 0));
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