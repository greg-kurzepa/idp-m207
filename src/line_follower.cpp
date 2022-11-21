#include "motor.hpp"


extern LineReading line_readings[] = {BlackLine, BlackLine, BlackLine, BlackLine};

// number of times each line sensor reading has changed since the last response
extern int line_changes[] = {0, 0, 0, 0};
// keep record of previous response in case the response got lost (need to retry)
extern int prev_line_changes[] = {0, 0, 0, 0};

void setup_follower() {
    pinMode(follower_1_pin, INPUT);
    pinMode(follower_2_pin, INPUT);
    pinMode(follower_3_pin, INPUT);
    pinMode(follower_4_pin, INPUT);
}

pin_size_t follower_pins[4] = {follower_1_pin,follower_2_pin,follower_3_pin,follower_4_pin};

void get_follower_readings() {
    for (size_t i = 0; i < 4; i++)
    {
        LineReading prev_reading = line_readings[i];
        if (!digitalRead(follower_pins[i])) {
            line_readings[i] = WhiteLine;
        } else {
            line_readings[i] = BlackLine;
        }
        if (line_readings[i] != prev_reading) {
            line_changes[i]++;
        }
    }
    
    
}