#include <WiFiManager.h>
#include "wifi.h"
#include "ble.h"
#include "constants.h"
#include "mpu.h"

float Kp = 2.0, Ki = 0.0, Kd = 1.0;
float setPoint = 0, input, output;
float errSum = 0, lastErr = 0;
unsigned long lastTime;

void setup() {
    Serial.begin(9600);
    initWIFI();
    initBLE();
}

int mapValue(int value, int inMin, int inMax, int outMin, int outMax) {
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void loop() {
    int16_t dx = read(dix);
    int16_t dy = read(diy);
    int16_t dz = read(diz);

    dx = mapValue(dx, 0, 1023, -512, 512);
    dy = mapValue(dy, 0, 1023, -512, 512);
    dz = mapValue(dz, 0, 1023, -512, 512);

    int16_t gx, gy, gz;
    getRotation(&gx, &gy, &gz);
    input = gx / 131.0;

    unsigned long now = millis();
    float timeChange = (float)(now - lastTime);

    float error = setPoint - input;
    errSum += (error * timeChange);
    float dErr = (error - lastErr) / timeChange;

    output = Kp * error + Ki * errSum + Kd * dErr;
    lastErr = error;
    lastTime = now;

    int baseSpeed = 128;
    int M[4] = {baseSpeed + mapValue(dy - dx + dz, -1536, 1536, -128, 128),
                baseSpeed + mapValue(dy + dx - dz, -1536, 1536, -128, 128),
                baseSpeed + mapValue(dy - dx - dz, -1536, 1536, -128, 128),
                baseSpeed + mapValue(dy + dx + dz, -1536, 1536, -128, 128)};
    for (int i = 0; i < 4; ++i) M[i] -= output;
    for (int i = 0; i < 4; ++i) M[i] = constrain(M[i], 0, 255);

    analogWrite(M1, M[0]);
    analogWrite(M2, M[1]);
    analogWrite(M3, M[2]);
    analogWrite(M4, M[3]);

    delay(20);
}
