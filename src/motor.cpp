
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS; // needed or bad things
Adafruit_DCMotor *motor1;
Adafruit_DCMotor *motor2;

int motor1_speed = 0;
int motor2_speed = 0;

void setup_motors() {
    AFMS = Adafruit_MotorShield();
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

    if (motor == nullptr) {
        Serial.println("NULLLLLLL!!!!!!");
    } else if (prev_speed != speed) {
        motor->setSpeed(speed);
        motor->run(BACKWARD);

        Serial.print("Motor ");
        Serial.print(n);
        Serial.print(" set to speed: ");
        Serial.println(speed);
    }
}
