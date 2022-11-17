#pragma once

#include <Arduino.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <Wire.h>

extern int max_looptime;

const int motion_led_pin = 0;
const int ultrasonic_trigger_pin = 1;
const int ultrasonic_1_pin = 2;
const int ultrasonic_2_pin = 3;
// on board, from right to left: purple, orange, yellow, brown
const int follower_1_pin = 4; // purple
const int follower_2_pin = 5; // yellow
const int follower_3_pin = 6; // orange
const int follower_4_pin = 7; // brown
// block density detection subsystem, all have prefix p_
const int p_is_dense_pin = 8; // green
const int p_is_block_pin = 9; // yellow
const int p_lo_density_led_pin = 10; // lights up green led
const int p_hi_density_led_pin = 11; // lights up red led

// Motors
void setup_motors();
void set_motor_speed(int n, int speed);
void pause_motors();
void resume_motors();
bool is_moving();

extern bool is_paused;

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

// LEDs
void update_leds();
void setup_leds();
bool detect_cube();

// Time
void update_time();
extern int prev_millis;
extern int curr_millis;