#include "core.hpp"

/// @brief Move cumulative unsent data from previous loops (previous snapshot) to current loop (current snapshot)
void combine_prev_response() {
    for (size_t i = 0; i < N_FOLLOWERS; i++) {
        line_changes[i] += prev_line_changes[i];
        prev_line_changes[i] = 0;
    }
}

/// @brief Store snapshot of cumulative data sent in latest reponse, and reset line_changes to start accumulating afresh for the next response
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

/// @brief Limits the maximum value of each value in line_changes to that of a 4 bit integer, for sending over the network
/// @param n index of value in line_changes array to operate on
uint8_t get_line_change(size_t n) {
    int nchanges = line_changes[n];
    int width = 4;
    int limit = (1<<width)-1;
    if (nchanges > limit) {
        nchanges = limit;
    }
    return nchanges;
}

/// @brief Decode message in recv_buffer, put response in send_buffer
/// @param recv_buffer 
/// @param send_buffer 
void handle_request(uint8_t recv_buffer[RECV_BUFSIZE], uint8_t send_buffer[SEND_BUFSIZE]) {
    //
    // Decode message and performs instructions
    // For data layout protocol data see documentation
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
    // (bit 1 & 2 for motor reverse, already used above)
    // bit 0 → set grabber position
    if (instruction & 1<<0) {
        close_grabber();
    } else {
        open_grabber();
    }

    // Bytes 1 & 2 for motor speed, already used above
    //

    // Byte 3 (block density)
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

    // if we know that the server (arduino) still hasn't managed to send data since the last response from the client...
    if (is_retrying) {
        Serial.println("Wifi: Retrying failed response");
        // ...combine previous line sensor changes with new ones to get the total line sensor changes since last response from client
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
    // 4 bits for line sensor data
    res_byte1 |= line_readings[0] << 4;
    res_byte1 |= line_readings[1] << 5;
    res_byte1 |= line_readings[2] << 6;
    res_byte1 |= line_readings[3] << 7;
    // 3 bits for block data
    res_byte1 |= detect_block_presence() << 3;
    res_byte1 |= determine_block_density() << 2;
    res_byte1 |= is_grabber_moving() << 1;
    send_buffer[0] = res_byte1;

    // take readings from ultrasonic sensors if client requested them
    if (get_ultrasonic_data) {
        pulse_ultrasonic(0);
        pulse_ultrasonic(1);
    }
    // fits the two 16-bit readings for each ultrasonic sensor into 4 bytes
    send_buffer[1] = latest_ultrasonic_dists[0] >> 8;
    send_buffer[2] = latest_ultrasonic_dists[0];
    send_buffer[3] = latest_ultrasonic_dists[1] >> 8;
    send_buffer[4] = latest_ultrasonic_dists[1];

    // fits the 4 values in line_changes into 2 bytes
    send_buffer[5] = (get_line_change(0) << 4) | get_line_change(1);
    send_buffer[6] = (get_line_change(2) << 4) | get_line_change(3);

    // resets line_changes array back to zero for next loop
    propagate_reponse();
}