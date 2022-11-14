
#include "motor.hpp"

// distance in mm; 0 reserved for timeout
uint16_t latest_ultrasonic1_dist = 0;
uint16_t latest_ultrasonic2_dist = 0;

void setup_ultrasonic() {
    pinMode(ultrasonic_trigger_pin, OUTPUT);
    pinMode(ultrasonic_1_pin, INPUT);
    pinMode(ultrasonic_2_pin, INPUT);
}

const int echo_timeout = 8000; // μs

void pulse_ultrasonic(int n) {
    // Trigger the pulses
    digitalWrite(ultrasonic_trigger_pin, LOW);
    delayMicroseconds(5);
    digitalWrite(ultrasonic_trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultrasonic_trigger_pin, LOW);

    int echo_pin;
    switch (n)
    {
    case 1:
        echo_pin = ultrasonic_1_pin;
        break;
    case 2:
        echo_pin = ultrasonic_2_pin;
        break;
    }
    long trip_micros = pulseIn(echo_pin, HIGH, echo_timeout);
    int dist_mm = ((speed_of_sound * trip_micros)/ 1000) / 2;
    
    switch (n)
    {
    case 1:
        latest_ultrasonic1_dist = dist_mm;
        break;
    case 2:
        latest_ultrasonic2_dist = dist_mm;
        break;
    }
}