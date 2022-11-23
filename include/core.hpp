#pragma once

#include <Arduino.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <Wire.h>

// number of line followers, ultrasonic sensors
#define N_FOLLOWERS 4
#define N_ULTRASONICS 2

const pin_size_t motion_led_pin = 0; // orange wire, amber LED
const pin_size_t lo_density_led_pin = 1; // green LED
const pin_size_t hi_density_led_pin = 2; // red LED
const pin_size_t block_present_pin = 3; // yellow wire
const pin_size_t block_is_dense_pin = 4; // green wire
const pin_size_t ultrasonic_trigger_pin = 5; // pink
const pin_size_t ultrasonic_pins[N_ULTRASONICS] = {6,7}; // echo pins
// connectors on board, from right to left: purple, orange, yellow, brown
// pin wire colours: purple, yellow, orange, brown
const pin_size_t follower_pins[N_FOLLOWERS] = {10,11,12,13};


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
void setup_followers();
void take_follower_readings();
void update_followers();

enum LineReading {
    BlackLine=0,
    WhiteLine=1,
};

extern LineReading line_readings[N_FOLLOWERS];
extern int line_changes[N_FOLLOWERS];
extern int prev_line_changes[N_FOLLOWERS];

// Ultrasonic
void setup_ultrasonic();
void pulse_ultrasonic(size_t n);

const int speed_of_sound = 340;

extern uint16_t latest_ultrasonic_dists[N_ULTRASONICS];

// WiFi
#define SERVER_PORT 23
#define RECV_BUFSIZE 6 // length of each message received from pc client
#define SEND_BUFSIZE 7 // length of each message sent to pc client

void setup_wifi();
void update_wifi();
void handle_request(uint8_t recv_buffer[RECV_BUFSIZE], uint8_t send_buffer[RECV_BUFSIZE]);

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

void update_grabber();
void open_grabber();
void close_grabber();
bool is_grabber_moving();

// Time
extern long prev_cycle_time;
extern long cur_cycle_time;