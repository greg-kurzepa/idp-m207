#include "motor.hpp"

bool motion_led_on = false;
int prev_toggle_millis = 0;
const int pulse_motion_period = 455; // minimum time inbetween toggle of the motion_led in ms

void setup_motion_led() {
    pinMode(motion_led_pin, OUTPUT);
    digitalWrite(motion_led_pin, 0);
}

void update_motion_led() {
    // This LED is on at 2Hz +- 10% if the robot is moving
    if (is_moving()) { // if motors are spinning, toggle led if necessary
        if (curr_millis - prev_toggle_millis >= pulse_motion_period) { // if led needs to be toggled (>=455ms has passed)
            prev_toggle_millis = curr_millis;

            motion_led_on = !motion_led_on;
            digitalWrite(motion_led_pin, motion_led_on);
        }
    } else if (motion_led_on) { // if motors are not spinning and led is on, turn off led
        motion_led_on = false;
        digitalWrite(motion_led_pin, 0);
    }
}
