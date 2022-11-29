
#include "core.hpp"

// distance in mm; 0 reserved for timeout
extern uint16_t latest_ultrasonic_dists[N_ULTRASONICS] = {0,0};

void setup_ultrasonic() {
    pinMode(ultrasonic_trigger_pin, OUTPUT);
    pinMode(ultrasonic_pins[0], INPUT);
    pinMode(ultrasonic_pins[1], INPUT);
}

const int echo_timeout = 8000; // μs

// Takes a distance measurement from one of the ultrasonic sensors
void pulse_ultrasonic(size_t idx) {
    // Trigger the pulses
    digitalWrite(ultrasonic_trigger_pin, LOW);
    delayMicroseconds(5);
    digitalWrite(ultrasonic_trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultrasonic_trigger_pin, LOW);

    long trip_micros = pulseIn(ultrasonic_pins[idx], HIGH, echo_timeout);
    int dist_mm = ((speed_of_sound * trip_micros)/ 1000) / 2;
    
    latest_ultrasonic_dists[idx] = dist_mm;
}