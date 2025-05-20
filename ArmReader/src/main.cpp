// * The serial communication will be replaced with BLE

#include <Arduino.h>

// The joystick's vertical axis' pin
#define J_PIN_Y A0
// The joystick's toggle button pin
#define J_PIN_B A1

uint16_t JYAxisInput;
byte JYAxisTransfer;
bool ButtonPressed;

void setup() {
    Serial.begin(9600);
    while(!Serial);

    pinMode(J_PIN_Y, INPUT);
    pinMode(J_PIN_B, INPUT);

    Serial.println("[AR]-< Initialized");
}

void loop() {
    JYAxisInput = analogRead(J_PIN_Y);
    ButtonPressed = analogRead(J_PIN_B) <= 100;

    JYAxisTransfer = map(JYAxisInput, 0, 1023, 0, 255);

    Serial.print("[+]-< Joystick Y-axis input: ");
    Serial.println(JYAxisInput);
    Serial.print("[+]-< Joystick Y-axis transfer: ");
    Serial.println(JYAxisTransfer);
    Serial.println(" |");

    // Writing one (first) byte to the serial port
    Serial.write(JYAxisTransfer);

    Serial.print("[+]-< Joystick button signal: ");
    Serial.println(ButtonPressed);
    Serial.println(" |");

    // Writing one (second) byte to the serial port
    Serial.write(ButtonPressed);

    delay(1000);
}