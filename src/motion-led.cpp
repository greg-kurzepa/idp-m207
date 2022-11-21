#include "motor.hpp"

bool motion_led_on = false;
int prev_cycle_time = -1; // ms
// measures time since light being turned on, excluding paused time
int phase = 0;
const int pulse_motion_period = 455; // minimum time inbetween toggle of the motion_led in ms

void setup_motion_led() {
    pinMode(motion_led_pin, OUTPUT);
    digitalWrite(motion_led_pin, 0);
    prev_cycle_time = millis();
}

void set_motion_led_on(bool on) {
    if (motion_led_on != on) {
        motion_led_on = on;
        digitalWrite(motion_led_pin, on);
    }
}

void update_motion_led() {
    // This LED is on at 2Hz +- 10% if the robot is moving
    if (is_moving()) {
        phase += (millis() - prev_cycle_time);
        if (phase < pulse_motion_period) {
            // For first half of cycle, turn LED on
            set_motion_led_on(true);
        } else if (phase < (2*pulse_motion_period)) {
            // For second half of cycle, turn LED off
            set_motion_led_on(false);
        } else {
            // Reset cycle
            phase = 0;
        }
    } else {
        set_motion_led_on(false);
    }
    prev_cycle_time = millis();
}