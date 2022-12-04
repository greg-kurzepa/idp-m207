
#include "core.hpp"

// distance in mm; 0 reserved for timeout (no response)
extern uint16_t latest_ultrasonic_dists[N_ULTRASONICS] = {0,0};

void setup_ultrasonic() {
    pinMode(ultrasonic_trigger_pin, OUTPUT);
    pinMode(ultrasonic_pins[0], INPUT);
    pinMode(ultrasonic_pins[1], INPUT);
}

const int echo_timeout = 8000; // μs

/// @brief Takes a distance measurement from one of the ultrasonic sensors
/// @param idx id of sensor to read from (0 or 1)
void pulse_ultrasonic(size_t idx) {
    // Trigger pulse of *both* ultrasonic sensors (both have their triggers connected to the same pin)
    digitalWrite(ultrasonic_trigger_pin, LOW);
    delayMicroseconds(5);
    digitalWrite(ultrasonic_trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultrasonic_trigger_pin, LOW);

    // Calculation of distance reading
    long trip_micros = pulseIn(ultrasonic_pins[idx], HIGH, echo_timeout);
    int dist_mm = ((speed_of_sound * trip_micros)/ 1000) / 2;
    latest_ultrasonic_dists[idx] = dist_mm;
}