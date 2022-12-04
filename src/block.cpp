#include "core.hpp"

// rack and pinion grabber constants
const int grabber_motor_close_speed = 255;
const int grabber_closing_duration = 3000; // ms
const int grabber_opening_duration = 3000; // ms

// time when grabber motion started (ms)
long grabber_start_time = 0;

// globally stored the current status of the grabber; whether it's open, closed, or currently opening or closing.
extern GrabberStatus grabber_status = GrabberOpen;

/// @brief Sets grabber_status to what status the grabber should be in to meet the demand from input parameters.
/// @param transient_status current thing the grabber is doing: GrabberClosing to close the grabber, or GrabberOpening to open it.
/// @param final_status final state of the grabber: GrabberClosed to close it, or GrabberOpen to open it.
/// @param speed speed the grabber should close/open at.
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

/// @brief Closes grabber using set_grabber_position().
void close_grabber() {
    set_grabber_position(GrabberClosing, GrabberClosed, grabber_motor_close_speed);
}

/// @brief Opens grabber using set_grabber_position().
void open_grabber() {
    set_grabber_position(GrabberOpening, GrabberOpen, -grabber_motor_close_speed);
}

/// @brief Checks if grabber is moving.
bool is_grabber_moving() {
    return (grabber_status == GrabberClosing) || (grabber_status == GrabberOpening);
}

/// @brief Should be run every timestep. Updates the grabber to meet demand by grabber_status.
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
    // If grabber has already been opening/closing for at least as long as the set opening/closing duration, stop moving it.
    if (duration && (duration <= (millis() - grabber_start_time))) {
        grabber_status = end_state;
        Serial.println("Grabber movement completed");
        set_motor_speed(grabber_motor_num, 0);
    }
}

/// @brief Sets up pins for red & green block density LEDs
void setup_block_leds() {
    pinMode(block_is_dense_pin, INPUT);
    pinMode(block_present_pin, INPUT);
    pinMode(lo_density_led_pin, OUTPUT);
    pinMode(hi_density_led_pin, OUTPUT);

    digitalWrite(lo_density_led_pin, 0);
    digitalWrite(hi_density_led_pin, 0);
}

// 
BlockDensity signalled_density = NilDensity;

/// @brief 
/// @return takes ldr reading and returns density of block as HighDensity or LowDensity.
BlockDensity determine_block_density() {
    if (digitalRead(block_is_dense_pin)) {
        return HighDensity;
    } else {
        return LowDensity;
    }
}

/// @brief 
/// @return takes ldr reading and returns whether a block is present or not.
bool detect_block_presence() {
    return digitalRead(block_present_pin) == HIGH;
}

/// @brief Output desired block density indication onto LEDs. Does not automatically time 5s, this is handled by the client.
/// @param density
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