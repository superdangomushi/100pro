#include <ArduinoBLE.h>

#define DEBUG_SERIAL
#define LOCKED_ANGLE 0
#define UNLOCKED_ANGLE 180
#define SERVO_PIN 0

const int LED_PIN = 15;
volatile bool deviceConnected = false;
int currentAngle = LOCKED_ANGLE;

BLEService servoService("19B10000-E8F2-537E-4F6C-D104768A1214"); // 適当なUUID

void setup() {
#ifdef DEBUG_SERIAL
    Serial.begin(115200);
#endif
    pinMode(LED_PIN, OUTPUT);

    if (!BLE.begin()) {
#ifdef DEBUG_SERIAL
        Serial.println("Starting BLE failed!");
#endif
        while (1);
    }

    BLE.setLocalName("BicycleLocker");
    BLE.setAdvertisedService(servoService);
    BLE.addService(servoService);
    BLE.advertise();
}

void loop() {
    BLEDevice central = BLE.central();

    if (central) {
#ifdef DEBUG_SERIAL
        Serial.printf("Connected to central: %s\n", central.address());
#endif
        deviceConnected = true;
        digitalWrite(LED_PIN, HIGH); // 接続中はLED点灯
        while (central.connected()) delay(100); //接続中はずっとここでループ
#ifdef DEBUG_SERIAL
        Serial.printf("Disconected from central: %s\n", central.address());
#endif
        deviceConnected = false;
        digitalWrite(LED_PIN, LOW);
    }
}

void setup1() {
    pinMode(SERVO_PIN, OUTPUT);
}

void loop1() {
    long startMicros = micros();
    currentAngle = controlAngle(currentAngle, deviceConnected ? UNLOCKED_ANGLE : LOCKED_ANGLE);
    controlServo(currentAngle, startMicros);
}

int controlAngle(int angle,int targetAngle) {
    if (angle == targetAngle) return angle;
    return (angle - targetAngle > 0) ? angle-- : angle++;
}

void controlServo(int angle, int startMicros) {
    long pulseWidth = map(angle, 0, 180, 500, 2400);
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
    delayMicroseconds(20000 - (micros() - startMicros));
}