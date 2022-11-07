#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

void setup_motors();
void set_motor_speed(int n, int speed);

void setup_follower();
void get_follower_readings();

void setup_wifi();
void tick_wifi();

namespace WifiSecrets {
    extern char ssid[];
    extern char pass[];
}