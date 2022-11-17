#define RECV_BUFSIZE 6 // length of each message received from pc client
#define SEND_BUFSIZE 7 // length of each message sent to pc client

#include "motor.hpp"
#include <WiFiNINA.h>

// remote control timeout (ms):
// After no messages after this duration,
// pause effects of remote commands
int rc_timeout = 100;
int last_insn_time = -1;
extern bool is_paused = false;

uint8_t last_request_id = 255;
// Used to combine cumulative data of latest response with the previous failed response
bool is_retrying = false;

int wifi_status = WL_IDLE_STATUS;
WiFiServer wifi_server(23); // arg1 = server port
// bool is_client_connected = false;

// Connect to mobile hotspot
void wifi_connect() {
    while (wifi_status != WL_CONNECTED) {
        Serial.print("Wifi: Connecting to SSID: ");
        Serial.println(WifiSecrets::ssid);
        wifi_status = WiFi.begin(WifiSecrets::ssid, WifiSecrets::pass);
        // wait for connection attempt
        while ((wifi_status = WiFi.status()) == WL_IDLE_STATUS) {
            delay(10);
        }
        // either WL_CONNECTED or WL_CONNECT_FAILED
    }
    Serial.print("Wifi: Connected from IP address: ");
    Serial.println(WiFi.localIP());

    wifi_server.begin();
}

void setup_wifi() {
    Serial.println("Wifi: Starting server");

    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("FATAL ERROR: Could not communicate with Wi-Fi module");
        while (true); // STOP
    }
}

void pause_activities() {
    is_paused = true;
    Serial.println("Wifi: Pausing due to no network activity");
    pause_motors();
}

void resume_activities() {
    is_paused = false;
    Serial.println("Wifi: Resuming activity due to reconnection");
    resume_motors();
}

void combine_prev_response() {
    line_changes[0] = line_changes[0] + prev_line_changes[0];
    line_changes[1] = line_changes[1] + prev_line_changes[1];
    line_changes[2] = line_changes[2] + prev_line_changes[2];
    line_changes[3] = line_changes[3] + prev_line_changes[3];
    prev_line_changes[0] = 0;
    prev_line_changes[1] = 0;
    prev_line_changes[2] = 0;
    prev_line_changes[3] = 0;
}

void propagate_reponse() {
    prev_line_changes[0] = line_changes[0];
    prev_line_changes[1] = line_changes[1];
    prev_line_changes[2] = line_changes[2];
    prev_line_changes[3] = line_changes[3];
    line_changes[0] = 0;
    line_changes[1] = 0;
    line_changes[2] = 0;
    line_changes[3] = 0;
}

void handle_request(WiFiClient client) {
    // Q: is client.flush() needed on first connection?
    // if (!is_client_connected) {
    //     is_client_connected = true;
    //     client.flush();
    // }

    uint8_t recv_buffer[RECV_BUFSIZE];
    uint8_t send_buffer[SEND_BUFSIZE];
    uint8_t instruction {0};
    uint8_t led_set {0};

    bool get_follower_data = false;
    bool get_ultrasonic_data = false;

    // read all bytes from client (assume fixed length message)
    client.read(&recv_buffer[0], RECV_BUFSIZE);

    // decodes message and performs instructions

    // byte 0 (instruction byte)
    instruction = recv_buffer[0];
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

    // byte 3 (set LEDs)
    led_set = recv_buffer[3];
    // TODO set some LEDs here

    // Byte 4: request id (cycles from 0 to 255)
    uint8_t id = recv_buffer[4];
    is_retrying = id == last_request_id;
    last_request_id = id;
    if (is_retrying) {
        combine_prev_response();
    }

    // Note: bytes 5,6 are currently unassigned to a purpose in life (so sad)

    propagate_reponse();

    //
    // Send response of sensor readings
    //
    // (CANNOT CURRENTLY SEND ANY IMU READINGS since we have not implemented imu stuff yet)
    
    if (get_follower_data) {
        get_follower_readings();
        uint8_t follower_data {0};
        follower_data |= follower_1 << 7;
        follower_data |= follower_2 << 6;
        follower_data |= follower_3 << 5;
        follower_data |= follower_4 << 4;
        send_buffer[0] = follower_data;
    }

    if (get_ultrasonic_data) {
        pulse_ultrasonic(1);
        pulse_ultrasonic(2);
    }
    send_buffer[1] = latest_ultrasonic1_dist >> 8;
    send_buffer[2] = latest_ultrasonic1_dist;
    send_buffer[3] = latest_ultrasonic2_dist >> 8;
    send_buffer[4] = latest_ultrasonic2_dist;

    send_buffer[5] = (line_changes[0] << 4) | line_changes[1];
    send_buffer[6] = (line_changes[2] << 4) | line_changes[3];

    wifi_server.write(send_buffer, SEND_BUFSIZE);
}

// call client.stop() after this many milliseconds
int client_timeout = 1000;
WiFiClient last_client;

int reset_timeout = 5000;
int keepalive_time;

void(* resetArduino) (void) = 0;

void tick_wifi() {
    wifi_status = WiFi.status();

    // Serial.println("Wifi status: "+wifi_status);
    // (Re)connect to access point if necessary
    if (wifi_status != WL_CONNECTED) {
        if (!is_paused) {
            pause_activities();
        }
        wifi_connect();
        keepalive_time = millis();
    }
    int current_millis = millis();

    // obtain a connected client that has available readable data.
    // client connection persists out of scope
    WiFiClient client = wifi_server.available();
    if (client && client.available() > 0) {
        int nbytes = client.available();
        if (nbytes >= RECV_BUFSIZE) {
            if (nbytes != RECV_BUFSIZE) {
                Serial.println("Warning: too much data");
            }
            last_insn_time = current_millis;
            keepalive_time = current_millis;
            // the below would get run somewhat regularly and cause occasional delays
            // if (client != last_client) {
            //     Serial.println("Wifi: Client replaced, disconnecting");
            //     last_client.stop();
            // }
            last_client = client;
            if (is_paused) {
                resume_activities();
            }
            handle_request(client);
        } else {
            Serial.print("OOPS NOT ENOUGH DATA: ");
            Serial.println(nbytes);
        }
    } else { // no client with data
        int time_since_res = (current_millis - last_insn_time);
        if (rc_timeout < time_since_res && !is_paused) {
            pause_activities();
        }
        if (client_timeout < time_since_res) {
            if (last_client.connected()) {
                Serial.println("Wifi: Disconnecting client");
                last_client.stop();
            }
        }
        if (reset_timeout < time_since_res) {
            Serial.println("Wifi: Resetting");
            resetArduino();
        }
    }
}