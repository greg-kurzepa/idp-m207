#include "motor.hpp"

bool motion_led_on = false;
int prev_toggle_millis = 0;
int density_on_millis = 0;
const int pulse_motion_period = 455; // minimum time inbetween toggle of the motion_led in ms
const int density_led_timeout = 6000; // amount of time density detection led stays on for after detecting the density

void setup_leds() {
    pinMode(motion_led_pin, OUTPUT);
    pinMode(p_is_dense_pin, INPUT);
    pinMode(p_is_block_pin, INPUT);
    pinMode(p_lo_density_led_pin, OUTPUT);
    pinMode(p_hi_density_led_pin, OUTPUT);

    digitalWrite(p_lo_density_led_pin, 0);
    digitalWrite(p_hi_density_led_pin, 0);
    digitalWrite(motion_led_pin, 0);
}

void update_leds() {

    // This LED is on at 2Hz +- 10% if the robot is moving
    if (is_moving()) { // if motors are spinning, toggle led if necessary
        if (curr_millis - prev_toggle_millis >= pulse_motion_period) { // if led needs to be toggled (>=455ms has passed)
            prev_toggle_millis = curr_millis;

            motion_led_on = !motion_led_on;
            digitalWrite(motion_led_pin, motion_led_on);
            Serial.println(motion_led_on);
        }
    } else if (motion_led_on) { // if motors are not spinning and led is on, turn off led
        motion_led_on = false;
        digitalWrite(motion_led_pin, 0);
        Serial.println(motion_led_on);
    }

    // This turns off the red/green LED if >=density_led_timeout ms has passed
    if (curr_millis - density_on_millis >= density_led_timeout) {
        digitalWrite(p_lo_density_led_pin, 0);
        digitalWrite(p_hi_density_led_pin, 0);
    }
}

// returns whether there is actually a block present or not
// should only be run once if it is to turn off after density_led_timeout ms
bool detect_cube() {
    if (digitalRead(p_is_block_pin)) { // if a block is present
        Serial.print("Block present! ");
        if (digitalRead(p_is_dense_pin)) { // if cube is dense, turn on red led
        Serial.print("Block dense!");
            digitalWrite(p_hi_density_led_pin, 1);
            density_on_millis = curr_millis;
        } else { // if cube is not dense, turn on green led
            Serial.print("Block not dense!");
            digitalWrite(p_lo_density_led_pin, 1);
            density_on_millis = curr_millis;
        }
        Serial.println();
        
        return 1;
    } else {
        return 0;
    }
}