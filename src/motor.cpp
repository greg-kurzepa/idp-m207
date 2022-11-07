
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_DCMotor *motor1;
Adafruit_DCMotor *motor2;

int motor1_speed = 0;
int motor2_speed = 0;

void setup_motors() {
    Adafruit_MotorShield AFMS = Adafruit_MotorShield();
    motor1 = AFMS.getMotor(1);
    motor2 = AFMS.getMotor(2);
    AFMS.begin();
}

void set_motor_speed(int n, int speed) {
    Adafruit_DCMotor *motor;
    int prev_speed = -1;
    switch (n)
    {
    case 1:
        prev_speed = motor1_speed;
        motor1_speed = speed;
        motor = motor1;
        break;
    case 2:
        prev_speed = motor2_speed;
        motor2_speed = speed;
        motor = motor2;
        break;
    default:
        break;
    }
    // Serial.println("about to set motor speeeedd to ");
    // Serial.println(speed);
    // Serial.print("of Motor ");
    // Serial.println(n);

    if (motor == nullptr) {
        Serial.println("NULLLLLLL!!!!!!");
    }
    delay(100);

    if (prev_speed != speed) {
        Serial.println("HERE WE GO");
        delay(100);
        motor->setSpeed(speed);
        Serial.println("SPEED HAS BEEN SETTTTT");
        delay(100);
        motor->run(BACKWARD);
        Serial.print("Motor ");
        Serial.print(n);
        Serial.print(" set to speed: ");
        Serial.println(speed);
    }
}
