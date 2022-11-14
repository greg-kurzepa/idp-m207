#pragma once

#include <Arduino.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <Wire.h>

const int ultrasonic_trigger_pin = 1;
const int ultrasonic_1_pin = 2;
const int ultrasonic_2_pin = 3;
// on board, from right to left: purple, orange, yellow, brown
const int follower_1_pin = 4; // purple
const int follower_2_pin = 5; // yellow
const int follower_3_pin = 6; // orange
const int follower_4_pin = 7; // brown

// Motors
void setup_motors();
void set_motor_speed(int n, int speed);
void pause_motors();
void resume_motors();

// Line followers
void setup_follower();
void get_follower_readings();

extern int follower_1;
extern int follower_2;
extern int follower_3;
extern int follower_4;
extern int line_changes[];
extern int prev_line_changes[];

// Ultrasonic
void setup_ultrasonic();
void pulse_ultrasonic(int n);

const int speed_of_sound = 340;

extern uint16_t latest_ultrasonic1_dist;
extern uint16_t latest_ultrasonic2_dist;

// WiFi
void setup_wifi();
void tick_wifi();

namespace WifiSecrets {
    extern char ssid[];
    extern char pass[];
}
