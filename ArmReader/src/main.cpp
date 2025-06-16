// * Note: This code is created out of pure blood and sweat of its author.
// * The basic functionality is identical to any other MPU6050 library.
// * I used Copilot to understand and to extend their code though :).
// * Here's the conversation that provided me the basic knowledge:
// * https://copilot.microsoft.com/shares/87RBiVezU2dXnzmMsvEa3

#include <Arduino.h>

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 Transmitter(CE_PIN, CSN_PIN);
const byte T_ADDRESS[6] = "1RF24";

// MPU6050s' preparation
#include <Wire.h>

// The MPUs' addresses to read I2C data from 
#define LOWER_MPU_ADR 0x68                  // This is standard address
#define UPPER_MPU_ADR 0x69                  // and this is modified

const int Addresses[2] = {LOWER_MPU_ADR, UPPER_MPU_ADR};

// The MPUs' ADO pins to tune the addresses
#define LOWER_MPU_ADO 2                     // This shouldn't be powered -> ADR = 0x68
#define UPPER_MPU_ADO 3                     // This should be powered -> ADR = 0x69

const short MPUsAmount = 2;
const short AxisAmount = 3;
// Arrays for accelerometers' and gyroscopes' values
float Accs[MPUsAmount][AxisAmount];
float Rots[MPUsAmount][AxisAmount];
// Array for acceleration's angles
float AccAngs[MPUsAmount][AxisAmount];

// Arrays for ouput angles
int16_t UpperAngles[AxisAmount];                // The angles of upper arm
int16_t LowerAngles[AxisAmount];                // The angles of underarm

// Correction variables
float AccMagnitude;                         // To determine whether MPU is moving or not
float Alphas[MPUsAmount];                   // To adjust acc/gyro influence upon movements
float GyroCorrs[MPUsAmount][AxisAmount];    // To allow fast rotations (high-pass)
float GyroBiases[MPUsAmount][AxisAmount];   // To reduce small drifts (low-pass)

#define TIMEOUT 10

// The joystick's vertical axis' pin
#define J_PIN_Y A1
// The joystick's toggle button pin
#define J_PIN_B A0

uint16_t JYAxisInput;
bool ButtonPressed;

// The data transfer array
int16_t TransArr[8];

void InitMPU(uint8_t);
void GetAngles(uint8_t, int16_t[3], bool);
void CorrectAngles(uint8_t, int16_t[3]);
void GetRawAccelerations(uint8_t);
void GetRawRotations(uint8_t);
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
    Wire.begin();

    Serial.println("[*]-< Initializing LowerMPU monitoring object");
    InitMPU(LOWER_MPU_ADR);

    Serial.println("[*]-< Initializing UpperMPU monitoring object");
    InitMPU(UPPER_MPU_ADR);

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
    Transmitter.setPayloadSize(sizeof(TransArr));
    Transmitter.setAutoAck(false);
    // Setting an amount and delay to retry failed transmissions
    Transmitter.setRetries(5, 3);
    // Setting the transmitter to send data
    Transmitter.openWritingPipe(T_ADDRESS);
    Transmitter.stopListening();

    Serial.println("[AR]-< Initialized");
}

void loop() {
    // Reading angles of the arm
    GetAngles(0, LowerAngles, true);

    TransArr[0] = LowerAngles[0];
    TransArr[1] = LowerAngles[1];
    TransArr[2] = LowerAngles[2];

    GetAngles(1, UpperAngles, true);

    TransArr[3] = LowerAngles[0];
    TransArr[4] = LowerAngles[1];
    TransArr[5] = LowerAngles[2];

    JYAxisInput = analogRead(J_PIN_Y);
    ButtonPressed = analogRead(J_PIN_B) <= 100;

    TransArr[6] = JYAxisInput;
    TransArr[7] = ButtonPressed;

    PrintData();
    // Writing one (first) byte to the radio channel
    if (Transmitter.write(&TransArr, sizeof(TransArr) * 2)) {
        Serial.println("Data sent");
    }

    delay(TIMEOUT);
}

void InitMPU(uint8_t adr) {
    Wire.beginTransmission(adr);
    // Talk to the register 6B and reset it by writing a 0
    Wire.write(0x6B);
    Wire.write(0x00);
    // End the transmission with a stop message
    Wire.endTransmission(true);
}

/// @brief Calculates rotation angles of a certain MPU module.
/// @param mpuNum - The number of an MPU module (either 0 or 1).
/// @param outArr - The output array to save calculations into.
/// @param corrReq - The flag to determine,
/// whether the angles should be automatically corrected or not.
void GetAngles(uint8_t mpuNum, int16_t (outArr)[3], bool corrReq) {
    // Reading MPU's raw data
    GetRawAccelerations(mpuNum);
    GetRawRotations(mpuNum);

    // Calculating angles in degrees (°)
    AccAngs[mpuNum][0] = atan(Accs[mpuNum][1] / sqrt(pow(Accs[mpuNum][0], 2) + pow(Accs[mpuNum][2], 2))) * 180 / PI;
    AccAngs[mpuNum][1] = atan(-(Accs[mpuNum][0]) / sqrt(pow(Accs[mpuNum][1], 2) + pow(Accs[mpuNum][2], 2))) * 180 / PI;
    // * Note: the Z-axis doesn't have any gravity reference, so its calculation can be omitted :)
    AccAngs[mpuNum][2] = atan((Accs[mpuNum][2]) / sqrt(pow(Accs[mpuNum][0], 2) + pow(Accs[mpuNum][1], 2))) * 180 / PI;

    if (corrReq) {
        CorrectAngles(mpuNum, outArr);
    } else {
        outArr[0] = (int16_t)round(AccAngs[mpuNum][0]);
        outArr[1] = (int16_t)round(AccAngs[mpuNum][1]);
        outArr[2] = (int16_t)round(AccAngs[mpuNum][2]);
    }
}
void CorrectAngles(uint8_t mpuNum, int16_t outArr[3]) {
    // Adjusting acc/gyro influence
    AccMagnitude = sqrt(pow(AccAngs[mpuNum][0], 2) + pow(AccAngs[mpuNum][1], 2) + pow(AccAngs[mpuNum][2], 2));
    // * Note: the accelerator has higher influence
    // * since the arm is considered to remain static most of the time
    Alphas[mpuNum] = abs(AccMagnitude - 1.0) > 0.1 ? 0.02 : 0.045;
    
    // Calculating high-passes
    GyroCorrs[mpuNum][0] = (outArr[0] - AccAngs[mpuNum][0]) * 0.8;
    GyroCorrs[mpuNum][1] = (outArr[1] - AccAngs[mpuNum][1]) * 0.8;
    GyroCorrs[mpuNum][2] = (outArr[2] - AccAngs[mpuNum][2]) * 0.8;
    // Calculating low-passes
    GyroBiases[mpuNum][0] += Rots[mpuNum][0] * 0.0001;
    GyroBiases[mpuNum][1] += Rots[mpuNum][1] * 0.0001;
    GyroBiases[mpuNum][2] += Rots[mpuNum][2] * 0.0001;

    outArr[0] = (int16_t)round(Alphas[mpuNum] * (outArr[0] + (Rots[mpuNum][0] - GyroBiases[mpuNum][0]) * TIMEOUT / 1000)
                 + (1 - Alphas[mpuNum]) * (AccAngs[mpuNum][0] + GyroCorrs[mpuNum][0]));
    outArr[1] = (int16_t)round(Alphas[mpuNum] * (outArr[1] + (Rots[mpuNum][1] - GyroBiases[mpuNum][1]) * TIMEOUT / 1000)
                 + (1 - Alphas[mpuNum]) * (AccAngs[mpuNum][1] + GyroCorrs[mpuNum][1]));
    outArr[2] = (int16_t)round(Alphas[mpuNum] * (outArr[2] + (Rots[mpuNum][2] - GyroBiases[mpuNum][2]) * TIMEOUT / 1000)
                 + (1 - Alphas[mpuNum]) * (AccAngs[mpuNum][2] + GyroCorrs[mpuNum][2]));
}

void GetRawAccelerations(uint8_t mpuNum) {
    Wire.beginTransmission(Addresses[mpuNum]);
    Wire.write(0x3B);
    Wire.endTransmission(true);
    Wire.requestFrom(Addresses[mpuNum], 6, true);

    // The division by 16384 (depends on sensitivity) is needed
    // to convert the sensor signals into physical values
    Accs[mpuNum][0] = (Wire.read() << 8 | Wire.read()) / 16384.0;
    Accs[mpuNum][1] = (Wire.read() << 8 | Wire.read()) / 16384.0;
    Accs[mpuNum][2] = (Wire.read() << 8 | Wire.read()) / 16384.0;
}
void GetRawRotations(uint8_t mpuNum) {
    Wire.beginTransmission(Addresses[mpuNum]);
    Wire.write(0x43);
    Wire.endTransmission(true);
    Wire.requestFrom(Addresses[mpuNum], 6, true);

    // The division by 131 (depends on sensitivity) is needed
    // to convert the sensor signals into physical values
    Rots[mpuNum][0] = (Wire.read() << 8 | Wire.read()) / 131.0;
    Rots[mpuNum][1] = (Wire.read() << 8 | Wire.read()) / 131.0;
    Rots[mpuNum][2] = (Wire.read() << 8 | Wire.read()) / 131.0;
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