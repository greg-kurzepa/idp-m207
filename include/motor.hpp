#pragma once

#include <Arduino.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <Wire.h>

extern int max_looptime;

const pin_size_t ultrasonic_trigger_pin = 1;
const pin_size_t ultrasonic_1_pin = 2;
const pin_size_t ultrasonic_2_pin = 3;
// on board, from right to left: purple, orange, yellow, brown
const pin_size_t follower_1_pin = 4; // purple
const pin_size_t follower_2_pin = 5; // yellow
const pin_size_t follower_3_pin = 6; // orange
const pin_size_t follower_4_pin = 7; // brown
// block density detection subsystem, all have prefix p_
const pin_size_t block_is_dense_pin = 8; // green wire
const pin_size_t block_present_pin = 9; // yellow wire
const pin_size_t lo_density_led_pin = 10; // lights up green led
const pin_size_t hi_density_led_pin = 11; // lights up red led
const pin_size_t motion_led_pin = 12; // orange wire

// Motors
const uint8_t left_motor_num = 1;
const uint8_t right_motor_num = 2;
const uint8_t grabber_motor_num = 3;

void setup_motors();
void set_motor_speed(int n, int speed);
void pause_motors();
void resume_motors();
bool is_moving();

extern bool is_paused;

// Line followers
void setup_follower();
void get_follower_readings();

enum LineReading {
    BlackLine=0,
    WhiteLine=1,
};

extern LineReading line_readings[4];
extern int line_changes[4];
extern int prev_line_changes[4];

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

// Motion LED
void update_motion_led();
void setup_motion_led();

// Block detection
enum BlockDensity {
    NilDensity  = -1,
    LowDensity  = 0,
    HighDensity = 1,
};

void setup_block_leds();
void update_block_leds();
void signal_block_density(BlockDensity density);
bool detect_block_presence();
BlockDensity determine_block_density();

// Grabber

enum GrabberStatus {
    GrabberClosing,
    GrabberOpening,
    GrabberClosed,
    GrabberOpen,
};
extern GrabberStatus grabber_status;

void setup_grabber();
void update_grabber();
void open_grabber();
void close_grabber();
bool is_grabber_moving();

// Time
void update_time();
extern long prev_millis;
extern long curr_millis;