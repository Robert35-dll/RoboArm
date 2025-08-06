#include <Arduino.h>

#pragma region [Radio Setup]

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

// The radio transmitter object for communication with another nRF24 module
RF24 Receiver(CE_PIN, CSN_PIN);
const byte R_ADDRESS[11] = "TRF24_ADDR";

int16_t TransArr[7];                // The data transfer array

#pragma endregion
#pragma region [Servos Setup]

// Servos' library
#include <Servo.h>

// Defining servos' pins
#define SX_PIN 3                    // Pin for shoulder's rotation at X-axis
#define SY_PIN 5                    // Pin for shoulder's rotation at Y-axis
#define SZ_PIN 6                    // Pin for shoulder's rotation at Z-axis
#define EL_PIN 9                    // Pin for elbow's bend
#define HA_PIN 10                   // Pin for hand's rotation along under arm

// Defining joints' servos
Servo SX;                           // The X-axis shoulder's servo
Servo SY;                           // The Y-axis shoulder's servo
Servo SZ;                           // The Z-axis shoulder's servo
Servo EL;                           // The elbow's servo
Servo HA;                           // The hand's servo

// Preparing angles' variables

const int AxisAmount = 3;
int16_t UpperAngles[AxisAmount];    // The angles of upper arm
int16_t LowerAngles[AxisAmount];    // The angles of underarm

int16_t JYAxisInput;                // The value of joystick's vertical input

#pragma endregion

#define TIMEOUT 10                  // The delay between measurements in milliseconds

void SetAngles();
void PrintData();
void PrintAngles(int16_t[3]);

#pragma region [Setup & Loop]

/// @brief The standard procedure called upon starting a microcontroller.
/// Hier happens the main setup of servos and radio transmitter.
void setup() {
    Serial.begin(9600);
    
    // Setting up the servos
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
    
    // Enabling the radio transmitter
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

/// @brief The standard procedure called repeatedly after finishing setup() procedure.
/// Hier the angles are being received via radio connection and written to servos.
void loop() {
    // If there's data to receive
    if (Receiver.available() > 0) {
        // Reading the transmitted bytes into an array
        Receiver.read(&TransArr, sizeof(TransArr) * 2);

        // * Note: the order of the angles is defined by the ArmReader
        // Updating angles and joystick values
        LowerAngles[0] = map(TransArr[0], -180, 180, 0, 180);
        LowerAngles[1] = map(TransArr[1], -180, 180, 0, 180);
        LowerAngles[2] = map(TransArr[2], -180, 180, 0, 180);

        UpperAngles[0] = map(TransArr[3], -180, 180, 0, 180);
        UpperAngles[1] = map(TransArr[4], -180, 180, 0, 180);
        UpperAngles[2] = map(TransArr[5], -180, 180, 0, 180);

        JYAxisInput = map(TransArr[6], 0, 1023, 0, 180);

        // * Keep in mind that printing data with small delay (< 100 ms) isn't really helpful :3
        // PrintData();

        // Writing the received angles to the servos
        SetAngles();

        delay(TIMEOUT);
    }
}

#pragma endregion
#pragma region [Angles Writing]

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

#pragma endregion
#pragma region [Miscellaneous]

void PrintData() {
    Serial.print("[+]-< Upper MPU angles: ");
    PrintAngles(UpperAngles);

    Serial.print("[+]-< Lower MPU angles: ");
    PrintAngles(LowerAngles);

    Serial.print("[+]-< Joystick Y-axis input: ");
    Serial.println(JYAxisInput);
    Serial.println(" |");
}
void PrintAngles(int16_t arr[3]) {
    Serial.print("X: ");
    Serial.print(arr[0]);
    Serial.print(" Y: ");
    Serial.print(arr[1]);
    Serial.print(" Z: ");
    Serial.println(arr[2]);
    Serial.println(" |");
}

#pragma endregion