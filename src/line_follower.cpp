#include "motor.hpp"

int follower_1_pin = 4; // left
int follower_2_pin = 5; // middle
int follower_3_pin = 7; // right
int follower_4_pin = 6;

extern int follower_1 = 0;
extern int follower_2 = 0;
extern int follower_3 = 0;
extern int follower_4 = 0;

void setup_follower() {
    pinMode(follower_1_pin, INPUT);
    pinMode(follower_2_pin, INPUT);
    pinMode(follower_3_pin, INPUT);
    pinMode(follower_4_pin, INPUT);
}

void get_follower_readings() {
    follower_1 = digitalRead(follower_1_pin);
    follower_2 = digitalRead(follower_2_pin);
    follower_3 = digitalRead(follower_3_pin);
    follower_4 = digitalRead(follower_4_pin);
}