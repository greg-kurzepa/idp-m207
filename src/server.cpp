

#include "core.hpp"

// Move cumulative data from previous snapshot to current snapshot
void combine_prev_response() {
    for (size_t i = 0; i < N_FOLLOWERS; i++) {
        line_changes[i] += prev_line_changes[i];
        prev_line_changes[i] = 0;
    }
}

// Store snapshot of cumulative data sent in latest reponse, and start
// accumulating afresh for the next response
void propagate_reponse() {
    for (size_t i = 0; i < N_FOLLOWERS; i++) {
        prev_line_changes[i] = line_changes[i];
        line_changes[i] = 0;
    }
}

// Each message has an ID cycling from 0 to 255 to help identify
// when data needs to be re-sent (in the event a response was lost)
uint8_t last_request_id = 255;
// Used to combine cumulative data of latest response with the previous failed response
bool is_retrying = false;

// Limits the line change data to 4 bits for sending over the network
uint8_t get_line_change(size_t n) {
    int nchanges = line_changes[n];
    int width = 4;
    int limit = (1<<width)-1;
    if (nchanges > limit) {
        nchanges = limit;
    }
    return nchanges;
}

void handle_request(uint8_t recv_buffer[RECV_BUFSIZE], uint8_t send_buffer[SEND_BUFSIZE]) {
    //
    // Decode message and performs instructions
    // =======================================================
    bool get_follower_data = false;
    bool get_ultrasonic_data = false;

    // Byte 0 (instruction byte)
    //
    uint8_t instruction = recv_buffer[0];

    // bit 7 → set motor 1 speed?
    if (instruction & 1<<7) {
        int speed = recv_buffer[1];
        // bit 2 → motor 1 reverse
        if (instruction & 1<<2) {
            speed = -speed;
        }
        set_motor_speed(1, speed);
    }
    // bit 6 → set motor 2 speed?
    if (instruction & 1<<6) {
        int speed = recv_buffer[2];
        // bit 1 → motor 2 reverse
        if (instruction & 1<<1) {
            speed = -speed;
        }
        set_motor_speed(2, speed);
    }
    // bit 5 → request line follower data?
    if (instruction & 1<<5) {
        get_follower_data = true;
    }
    // bit 4 → request ultrasonic data?
    if (instruction & 1<<4) {
        get_ultrasonic_data = true;
    }
    // (bit 1 & 2 for motor reverse)
    // bit 0 → set grabber position
    if (instruction & 1<<0) {
        close_grabber();
    } else {
        open_grabber();
    }

    // [Bytes 1 & 2 for motor speed]

    // Byte 3
    //
    int byte3 = recv_buffer[3];
    // bit 7 → signal density?
    if (byte3 & 1<<7) {
        // bit 6 → high density?
        if (byte3 & 1<<6) {
            signal_block_density(HighDensity);
        } else {
            signal_block_density(LowDensity);
        }
    } else {
        signal_block_density(NilDensity);
    }

    // Byte 4: request ID
    //
    uint8_t id = recv_buffer[4];
    is_retrying = (id == last_request_id);
    last_request_id = id;

    if (is_retrying) {
        Serial.println("Wifi: Retrying failed response");
        combine_prev_response();
    }

    // Note: Bytes 5,6 are currently unused

    //
    // Send response of sensor readings
    // ===============================================================
    
    send_buffer[0] = 0;
    // Redundant as follower readings taken every local cycle:
    // if (get_follower_data) {
    //     take_follower_readings();
    // }
    uint8_t res_byte1 = 0;
    res_byte1 |= line_readings[0] << 7;
    res_byte1 |= line_readings[1] << 6;
    res_byte1 |= line_readings[2] << 5;
    res_byte1 |= line_readings[3] << 4;
    res_byte1 |= detect_block_presence() << 3;
    res_byte1 |= determine_block_density() << 2;
    res_byte1 |= is_grabber_moving() << 1;
    send_buffer[0] = res_byte1;

    if (get_ultrasonic_data) {
        pulse_ultrasonic(0);
        pulse_ultrasonic(1);
    }
    send_buffer[1] = latest_ultrasonic_dists[0] >> 8;
    send_buffer[2] = latest_ultrasonic_dists[0];
    send_buffer[3] = latest_ultrasonic_dists[1] >> 8;
    send_buffer[4] = latest_ultrasonic_dists[1];

    send_buffer[5] = (get_line_change(0) << 4) | get_line_change(1);
    send_buffer[6] = (get_line_change(2) << 4) | get_line_change(3);

    propagate_reponse();
}