#include "core.hpp"

// The amber LED blinks at 2Hz +- 10% if the robot is moving

// minimum time inbetween toggle of the motion_led in ms
// aim for lower bound of period of desired 2Hz +- 10% frequency to avoid hitting upper bound during long loop times
const int half_period = 455;

bool motion_led_on = false;
// measures time since light being turned on, excluding paused time
// keeping track of phase so that the blink is smoother in spite of regular short pauses
int phase = 0;

void setup_motion_led() {
    pinMode(motion_led_pin, OUTPUT);
    digitalWrite(motion_led_pin, 0);
}

/// @brief Turn led on or off.
/// @param on true to turn on, false to turn off
void set_motion_led_on(bool on) {
    if (motion_led_on != on) {
        motion_led_on = on;
        digitalWrite(motion_led_pin, on);
    }
}

/// @brief Should be run every timestep. Updates the motion LED to its correct state, depending on current phase and whether the robot is moving
void update_motion_led() {
    if (is_moving()) {
        phase += (cur_cycle_time - prev_cycle_time);
        // Obtain principal phase [0,T)
        phase %= (2*half_period);

        if (phase < half_period) {
            // For first half of cycle, turn LED on
            set_motion_led_on(true);
        } else {
            // For second half of cycle, turn LED off
            set_motion_led_on(false);
        }
    } else {
        set_motion_led_on(false);
    }
    prev_cycle_time = cur_cycle_time;
}