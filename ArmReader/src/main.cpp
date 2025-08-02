// * Note: This code is created out of pure blood and sweat of its author.
// * The basic functionality is identical to any other MPU6050 library.
// * I used Copilot to understand and to extend their code though :).
// * Here's the conversation that provided me the basic knowledge:
// * https://copilot.microsoft.com/shares/87RBiVezU2dXnzmMsvEa3

#include <Arduino.h>

#pragma region [Radio Setup]

// Radio libraries
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

// The radio transmitter object for communication with another nRF24 module
RF24 Transmitter(CE_PIN, CSN_PIN);
const byte T_ADDRESS[11] = "TRF24_ADDR";

int16_t TransArr[7];                        // The data transfer array

#pragma endregion
#pragma region [MPU6050 Setup]

// Essential Wire library for reading MPUs' data
#include <Wire.h>

// The MPUs' addresses to read I2C data from 
#define UPPER_MPU_ADR 0x68                  // This is standard address
#define LOWER_MPU_ADR 0x69                  // This is modified address

const int Addresses[2] = {LOWER_MPU_ADR, UPPER_MPU_ADR};

// The MPUs' ADO pins to tune the addresses
#define UPPER_MPU_ADO 2                     // This shouldn't be powered -> ADR = 0x68
#define LOWER_MPU_ADO 3                     // This should be powered -> ADR = 0x69

// Preparing measurements' variables

const short MPUsAmount = 2;
const short AxisAmount = 3;

float Accs[MPUsAmount][AxisAmount];         // Array for accelerometers' values
float Rots[MPUsAmount][AxisAmount];         // Array for gyroscopes' values
float AccAngs[MPUsAmount][AxisAmount];      // Array for acceleration's angles

// Arrays for ouput angles
int16_t UpperAngles[AxisAmount];            // The angles of upper arm
int16_t LowerAngles[AxisAmount];            // The angles of underarm
int16_t UpperOffsets[AxisAmount];           // The initial angles of upper arm
int16_t LowerOffsets[AxisAmount];           // The initial angles of underarm

// Correction variables
float AccMagnitude;                         // To determine whether MPU is moving or not
float Alphas[MPUsAmount];                   // To adjust acc/gyro influence upon movements
float GyroCorrs[MPUsAmount][AxisAmount];    // To allow fast rotations (high-pass)
float GyroBiases[MPUsAmount][AxisAmount];   // To reduce small drifts (low-pass)

#pragma endregion
#pragma region [Joystick Setup]

#define J_PIN_Y A1

int16_t JYAxisInput;                        // The value of joystick's vertical input

#pragma endregion

#define TIMEOUT 10                          // The delay between measurements in milliseconds

void InitMPU(uint8_t);
void SetOffsets(uint8_t, int16_t[3]);
void GetAngles(uint8_t, int16_t[3], bool);
void CorrectAngles(uint8_t, int16_t[3]);
void GetRawAccelerations(uint8_t);
void GetRawRotations(uint8_t);
void PrintData();
void PrintAngles(int16_t[3]);

#pragma region [Setup & Loop]

/// @brief The standard procedure called upon starting a microcontroller.
/// Hier happens the main setup of MPUs and radio transmitter.
void setup() {
    Serial.begin(115200);

    Serial.println("[AR]-< Started initialization");
    // Setting up the MPUs
    pinMode(UPPER_MPU_ADO, OUTPUT);
    pinMode(LOWER_MPU_ADO, OUTPUT);
    // Alternating the address of one of them for correct initializing
    digitalWrite(UPPER_MPU_ADO, LOW);
    digitalWrite(LOWER_MPU_ADO, HIGH);

    // Joining the I2C bus
    Wire.begin();

    // Initializing MPUs and calculating their initial offsets
    Serial.println("[*]-< Initializing LowerMPU monitoring object");
    InitMPU(LOWER_MPU_ADR);
    SetOffsets(0, LowerAngles);
    LowerOffsets[0] = LowerAngles[0];
    LowerOffsets[1] = LowerAngles[1];
    LowerOffsets[2] = LowerAngles[2];

    Serial.println("[*]-< Initializing UpperMPU monitoring object");
    InitMPU(UPPER_MPU_ADR);
    SetOffsets(1, UpperAngles);
    UpperOffsets[0] = UpperAngles[0];
    UpperOffsets[1] = UpperAngles[1];
    UpperOffsets[2] = UpperAngles[2];

    // Setting up the joystick
    pinMode(J_PIN_Y, INPUT);

    // Enabling the radio transmitter
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
    Transmitter.setAutoAck(true);
    // Setting an amount and delay to retry failed transmissions
    Transmitter.setRetries(5, 3);
    // Setting the transmitter to send data
    Transmitter.openWritingPipe(T_ADDRESS);
    Transmitter.stopListening();

    Serial.println("[AR]-< Initialized");
}

/// @brief The standard procedure called repeatedly after finishing setup() procedure.
/// Hier the angles are being measured and transmitted via radio connection.
void loop() {
    // Reading and correcting angles of the underarm
    GetAngles(0, LowerAngles, true);

    TransArr[0] = LowerAngles[0] - LowerOffsets[0];
    TransArr[1] = LowerAngles[1] - LowerOffsets[1];
    TransArr[2] = LowerAngles[2] - LowerOffsets[2];
    
    // Reading and correcting angles of the upper arm
    GetAngles(1, UpperAngles, true);

    TransArr[3] = UpperAngles[0] - UpperOffsets[0];
    TransArr[4] = UpperAngles[1] - UpperOffsets[1];
    TransArr[5] = UpperAngles[2] - UpperOffsets[2];

    // Reading joystick's values
    JYAxisInput = analogRead(J_PIN_Y);

    TransArr[6] = JYAxisInput;

    // * Keep in mind that printing data with small delay (< 100 ms) isn't really helpful :3
    // PrintData();

    // Sending the data array to the radio channel
    if (Transmitter.write(&TransArr, sizeof(TransArr), false)) {
        Serial.println("Data sent");
    }

    delay(TIMEOUT);
}

#pragma endregion
#pragma region [Initialization]

/// @brief Initializes the connection to an MPU module.
/// @param adr - The address of the MPU to communicate with.
void InitMPU(uint8_t adr) {
    Wire.beginTransmission(adr);
    // Talk to the register 6B and reset it by writing a 0
    Wire.write(0x6B);
    Wire.write(0x00);
    // End the transmission with a stop message
    Wire.endTransmission(true);
}
/// @brief Reads the starting angles of an MPU and saves them as initial offsets.
/// @param mpuNum - The number of an MPU defined by the order in the Addresses' array.
/// @param offArr - An array to save offsets into.
void SetOffsets(uint8_t mpuNum, int16_t (offArr)[3]) {
    Serial.println("Calculating offsets. Don't move the MPU!");

    for (int i = 0; i < 1200; i++) {
        GetAngles(mpuNum, offArr, true);
        delay(TIMEOUT);
    }

    Serial.println("Finished calculating offsets!");
    PrintAngles(offArr);
}

#pragma endregion
#pragma region [Angles Reading]

/// @brief Calculates rotation angles of a certain MPU module.
/// @param mpuNum - The number of an MPU defined by the order in the Addresses' array.
/// @param outArr - An array to save calculations into.
/// @param corrReq - The flag to determine, whether the angles should be corrected or not.
void GetAngles(uint8_t mpuNum, int16_t (outArr)[3], bool corrReq) {
    // Reading MPU's raw data
    GetRawAccelerations(mpuNum);
    GetRawRotations(mpuNum);

    // Calculating angles in degrees (°)
    AccAngs[mpuNum][0] = atan(Accs[mpuNum][1] / sqrt(pow(Accs[mpuNum][0], 2) + pow(Accs[mpuNum][2], 2))) * 180 / PI;
    AccAngs[mpuNum][1] = atan(-(Accs[mpuNum][0]) / sqrt(pow(Accs[mpuNum][1], 2) + pow(Accs[mpuNum][2], 2))) * 180 / PI;
    // * Note: the Z-axis doesn't have any gravity reference, so its calculation can be omitted :)
    AccAngs[mpuNum][2] = atan((Accs[mpuNum][2]) / sqrt(pow(Accs[mpuNum][0], 2) + pow(Accs[mpuNum][1], 2))) * 180 / PI;

    // Correcting angles if requested
    if (corrReq) {
        CorrectAngles(mpuNum, outArr);
    } else {
        outArr[0] = (int16_t)round(AccAngs[mpuNum][0]);
        outArr[1] = (int16_t)round(AccAngs[mpuNum][1]);
        outArr[2] = (int16_t)round(AccAngs[mpuNum][2]);
    }
}
/// @brief Adjusts angles' values depending on their change rate.
/// @param mpuNum - The number of an MPU defined by the order in the Addresses' array.
/// @param outArr - An array to save calculations into.
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

    // and finally correcting the angles
    outArr[0] = (int16_t)round(Alphas[mpuNum] * (outArr[0] + (Rots[mpuNum][0] - GyroBiases[mpuNum][0]) * TIMEOUT / 1000)
                 + (1 - Alphas[mpuNum]) * (AccAngs[mpuNum][0] + GyroCorrs[mpuNum][0]));
    outArr[1] = (int16_t)round(Alphas[mpuNum] * (outArr[1] + (Rots[mpuNum][1] - GyroBiases[mpuNum][1]) * TIMEOUT / 1000)
                 + (1 - Alphas[mpuNum]) * (AccAngs[mpuNum][1] + GyroCorrs[mpuNum][1]));
    outArr[2] = (int16_t)round(Alphas[mpuNum] * (outArr[2] + (Rots[mpuNum][2] - GyroBiases[mpuNum][2]) * TIMEOUT / 1000)
                 + (1 - Alphas[mpuNum]) * (AccAngs[mpuNum][2] + GyroCorrs[mpuNum][2]));
}

/// @brief Reads raw accelerometer's data of an MPU module.
/// @param mpuNum - The number of an MPU defined by the order in the Addresses' array.
void GetRawAccelerations(uint8_t mpuNum) {
    // Telling the MPU to transmit accelerometer's data (stored at 0x3B address)
    Wire.beginTransmission(Addresses[mpuNum]);
    Wire.write(0x3B);
    Wire.endTransmission(true);
    // Requesting those data (6 bytes)
    Wire.requestFrom(Addresses[mpuNum], 6, true);

    // * The division by 16384 (depends on sensitivity) is needed
    // * to convert the sensor signals into physical values
    Accs[mpuNum][0] = (Wire.read() << 8 | Wire.read()) / 16384.0;
    Accs[mpuNum][1] = (Wire.read() << 8 | Wire.read()) / 16384.0;
    Accs[mpuNum][2] = (Wire.read() << 8 | Wire.read()) / 16384.0;
}
/// @brief Reads raw gyroscope's data of an MPU module.
/// @param mpuNum - The number of an MPU defined by the order in the Addresses' array.
void GetRawRotations(uint8_t mpuNum) {
    // Telling the MPU to transmit gyroscope's data (stored at 0x43 address)
    Wire.beginTransmission(Addresses[mpuNum]);
    Wire.write(0x43);
    Wire.endTransmission(true);
    // Requesting those data (6 bytes)
    Wire.requestFrom(Addresses[mpuNum], 6, true);

    // * The division by 131 (depends on sensitivity) is needed
    // * to convert the sensor signals into physical values
    Rots[mpuNum][0] = (Wire.read() << 8 | Wire.read()) / 131.0;
    Rots[mpuNum][1] = (Wire.read() << 8 | Wire.read()) / 131.0;
    Rots[mpuNum][2] = (Wire.read() << 8 | Wire.read()) / 131.0;
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