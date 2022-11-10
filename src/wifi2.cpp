#define RECV_BUFSIZE 6 // length of each message received from pc client
#define SEND_BUFSIZE 1 // length of each message sent to pc client

#include "motor.hpp"
#include <WiFiNINA.h>

// remote control timeout (ms)
// After no messages after this duration, assume disconnection
// and pause effects of remote commands
int rc_timeout = 200;
int last_insn_time = -1;
bool is_paused = false;

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
            delay(100);
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
    Serial.println("Wifi: Resuming activity due to reconnection");
    resume_motors();
}

void tick_wifi() {
    wifi_status = WiFi.status();

    // Serial.println("Wifi status: "+wifi_status);
    // (Re)connect to access point if necessary
    if (wifi_status != WL_CONNECTED) {
        if (!is_paused) {
            pause_activities();
        }
        wifi_connect();
    }

    int current_millis = millis();

    // obtain a connected client that has available readable data
    if (WiFiClient client = wifi_server.available()) {
        last_insn_time = current_millis;
        if (is_paused) {
            is_paused = false;
            resume_activities();
        }

        // Q: is client.flush() needed on first connection?
        // if (!is_client_connected) {
        //     is_client_connected = true;
        //     client.flush();
        // }

        uint8_t recv_buffer[RECV_BUFSIZE];
        uint8_t send_buffer[SEND_BUFSIZE];
        uint8_t instruction {0};
        uint8_t led_set {0};
        uint8_t follower_data {0};

        bool get_follower_data = false;
        bool get_imu_data = false;

        // read all bytes from client (assume fixed length message)
        client.read(&recv_buffer[0], RECV_BUFSIZE);
        // Serial.println(recv_buffer[0]);

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
            Serial.println("Requested line follower data");
            get_follower_data = true;
        }
        // bit 4 → request line follower data?
        if (instruction & 1<<4) {
            Serial.println("Requested IMU data");
            get_imu_data = true;
        }

        // byte 3 (set LEDs)
        led_set = recv_buffer[3];
        // TODO set some LEDs here

        // Note: bytes 4,5,6 are currently unassigned to a purpose in life (so sad)


        // Send response of sensor readings
        // (CANNOT CURRENTLY SEND ANY IMU READINGS since we have not implemented imu stuff yet)
        follower_data |= follower_1 << 7;
        follower_data |= follower_2 << 6;
        follower_data |= follower_3 << 5;
        follower_data |= follower_4 << 4;
        send_buffer[0] = follower_data;

        wifi_server.write(&send_buffer[0], SEND_BUFSIZE);

    } else { // no client with data
        if (rc_timeout < (current_millis - last_insn_time) && !is_paused) {
            pause_activities();
        }
    }
}