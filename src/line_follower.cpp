#include "core.hpp"


extern LineReading line_readings[] = {BlackLine, BlackLine, BlackLine, BlackLine};

// number of times each line sensor reading has changed since the last response
extern int line_changes[] = {0, 0, 0, 0};
// keep record of previous response in case the response got lost (need to retry)
extern int prev_line_changes[] = {0, 0, 0, 0};

void setup_followers() {
    for (size_t i = 0; i < N_FOLLOWERS; i++) {
        pinMode(follower_pins[i], INPUT);
    }
}

void take_follower_readings() {
    for (size_t i = 0; i < N_FOLLOWERS; i++) {
        LineReading prev_reading = line_readings[i];
        LineReading current_reading;
        if (digitalRead(follower_pins[i])) {
            current_reading = BlackLine;
        } else {
            current_reading = WhiteLine;
        }
        line_readings[i] = current_reading;
        if (current_reading != prev_reading) {
            line_changes[i]++;
        }
    }
}

void update_followers() {
    take_follower_readings();
}