
#include "core.hpp"

int grabber_motor_close_speed = 255;
int grabber_closing_duration = 3000; // ms
int grabber_opening_duration = 3000; // ms
long grabber_start_time = 0; // when motion started (ms)

extern GrabberStatus grabber_status = GrabberOpen;

void set_grabber_position(GrabberStatus transient_status, GrabberStatus final_status, int speed) {
    if (grabber_status == transient_status ||
        grabber_status == final_status) {
        return;
    }
    Serial.println("Starting grabber movement");
    grabber_status = transient_status;
    set_motor_speed(grabber_motor_num, speed);
    grabber_start_time = millis();
}

void close_grabber() {
    set_grabber_position(GrabberClosing, GrabberClosed, grabber_motor_close_speed);
}

void open_grabber() {
    set_grabber_position(GrabberOpening, GrabberOpen, -grabber_motor_close_speed);
}

bool is_grabber_moving() {
    return (grabber_status == GrabberClosing) || (grabber_status == GrabberOpening);
}

void update_grabber() {
    int duration = 0;
    GrabberStatus end_state;
    switch (grabber_status)
    {
    case GrabberClosing:
        duration = grabber_closing_duration;
        end_state = GrabberClosed;
        break;
    case GrabberOpening:
        duration = grabber_opening_duration;
        end_state = GrabberOpen;
        break;
    default:
        break;
    } 
    if (duration && (duration <= (millis() - grabber_start_time))) {
        grabber_status = end_state;
        Serial.println("Grabber movement completed");
        set_motor_speed(grabber_motor_num, 0);
    }
}

void setup_block_leds() {
    pinMode(block_is_dense_pin, INPUT);
    pinMode(block_present_pin, INPUT);
    pinMode(lo_density_led_pin, OUTPUT);
    pinMode(hi_density_led_pin, OUTPUT);

    digitalWrite(lo_density_led_pin, 0);
    digitalWrite(hi_density_led_pin, 0);
}

BlockDensity signalled_density = NilDensity;

BlockDensity determine_block_density() {
    if (digitalRead(block_is_dense_pin)) {
        return HighDensity;
    } else {
        return LowDensity;
    }
}

void signal_block_density(BlockDensity density) {
    if (signalled_density == density) {
        return;
    }
    signalled_density = density;
    switch (density)
    {
    case LowDensity:
        digitalWrite(hi_density_led_pin, 0);
        digitalWrite(lo_density_led_pin, 1);
        break;
    case HighDensity:
        digitalWrite(hi_density_led_pin, 1);
        digitalWrite(lo_density_led_pin, 0);
        break;
    case NilDensity:
        digitalWrite(hi_density_led_pin, 0);
        digitalWrite(lo_density_led_pin, 0);
        break;
    default:
        break;
    }
}

// returns whether there is actually a block present or not
bool detect_block_presence() {
    return digitalRead(block_present_pin) == HIGH;
}