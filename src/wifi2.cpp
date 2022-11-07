#define RECV_BUFSIZE 6 // length of each message received from pc client
#define SEND_BUFSIZE 1 // length of each message sent to pc client

#include "motor.hpp"
#include <WiFiNINA.h>

int wifi_status = WL_IDLE_STATUS;
WiFiServer wifi_server(23); // arg1 = server port
bool is_client_connected = false;

void setup_wifi() {
    Serial.println("Wifi: Starting server");

    if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERR: Could not communicate with Wi-Fi module");
    while (true); // STOP
    }

    // Connect to mobile hotspot
    while (wifi_status != WL_CONNECTED) {
    Serial.print("Wifi: Connecting to SSID: ");
    Serial.println(WifiSecrets::ssid);
    wifi_status = WiFi.begin(WifiSecrets::ssid, WifiSecrets::pass);
    delay(3000); // wait for connection
    }
    wifi_server.begin();

    IPAddress ip = WiFi.localIP();
    Serial.println("Wifi: Connected from IP address:");
    Serial.println(ip);
}

void tick_wifi() {
    WiFiClient client = wifi_server.available();
    uint8_t recv_buffer[RECV_BUFSIZE];
    uint8_t send_buffer[SEND_BUFSIZE];
    int send_buffer_idx = 0;
    uint8_t instruction {0};
    uint8_t led_set {0};
    uint8_t follower_data {0};

    bool get_follower_data = false;
    bool get_imu_data = false;

    if (client) {
        if (!is_client_connected) {
            is_client_connected = true;
            client.flush();
            // discard whatever bytes come in at the start of connection
            while (client.available()) {
                client.read();
            }
            //wifi_server.println("Welcome to the Fourth Wheel");
        }

        if (client.available() > 0) {
            client.read(&recv_buffer[0], RECV_BUFSIZE); // reads all bytes from client

            // decodes message and performs instructions

            // byte 0 (instruction byte)
            instruction = recv_buffer[0];
            if (instruction & 0b10000000) { // if 0th bit of instruction byte = 1 (i.e. = true):
                set_motor_speed(1, recv_buffer[1]);
            }
            if (instruction & 0b01000000) { // if 1st bit of instruction byte = 1 (i.e. = true):
                set_motor_speed(2, recv_buffer[1]);
            }
            if (instruction & 0b00100000) { // 2nd bit
                get_follower_data = true;
            }
            if (instruction & 0b00010000) { // 2nd bit
                get_imu_data = true;
            }

            // bytes 1 and 2 are for motor speeds

            // byte 3 (set LEDs)
            led_set = recv_buffer[3];
            // set some LEDs here

            // bytes 4,5,6 are currently unassigned to a purpose in life (so sad)

            // sends data back (CAN CURRENTLY NOT SEND ANY IMU READINGS since we have not implemented imu stuff yet)
            follower_data |= follower_1 << 7;
            follower_data |= follower_2 << 6;
            follower_data |= follower_3 << 5;
            follower_data |= follower_4 << 4;
            send_buffer[send_buffer_idx] = follower_data;

            wifi_server.write(&send_buffer[0], SEND_BUFSIZE);

        } else {
            Serial.println("Client not available!");
        }
    }
}