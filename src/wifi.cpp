
#include <WiFiNINA.h>
#include "core.hpp"

int wifi_status = WL_IDLE_STATUS;
WiFiServer wifi_server(SERVER_PORT);

// Connect to mobile hotspot, blocking until successful
void wifi_connect_wait() {
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
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("FATAL ERROR: Could not communicate with Wi-Fi module");
        while (true); // STOP
    }
}

// call client.stop() after this many milliseconds
const int client_timeout = 1000;
WiFiClient last_client;

// Restart Arduino if `reset_timeout` milliseconds elapsed since `keepalive_time`
// Used as a 'just-in-case' defensive measure if connection problems arise.
const int reset_timeout = 5000;
long keepalive_time = millis();

void(* resetArduino) (void) = 0;

// remote control timeout (ms):
// After no messages after this duration,
// pause effects of remote commands
const int rc_timeout = 100;
int last_req_time = -1;
extern bool is_paused = false;

void pause_activities() {
    is_paused = true;
    // Serial.println("Wifi: Pausing due to no network activity");
    pause_motors();
}

void resume_activities() {
    is_paused = false;
    // Serial.println("Wifi: Resuming activity due to reconnection");
    resume_motors();
}

void update_wifi() {
    wifi_status = WiFi.status();

    // (Re)connect to access point if necessary
    if (wifi_status != WL_CONNECTED) {
        if (!is_paused) {
            pause_activities();
        }
        wifi_connect_wait();
        keepalive_time = millis();
    }
    long current_time = millis();

    // obtain a connected client that has available readable data.
    // client connection persists out of scope
    WiFiClient client = wifi_server.available();
    if (client) {
        last_client = client;
    }
    if (client && (client.available() > 0)) {
        int nbytes = client.available();
        if (nbytes >= RECV_BUFSIZE) {
            if (nbytes != RECV_BUFSIZE) {
                Serial.println("Warning: too much data");
            }
            last_req_time = current_time;
            keepalive_time = current_time;
            if (is_paused) {
                resume_activities();
            }

            uint8_t recv_buffer[RECV_BUFSIZE];
            uint8_t send_buffer[SEND_BUFSIZE];

            // read all bytes from client (assume fixed length message)
            client.read(&recv_buffer[0], RECV_BUFSIZE);
            handle_request(recv_buffer, send_buffer);
            wifi_server.write(send_buffer, SEND_BUFSIZE);
        } else {
            Serial.print("Wifi: OOPS NOT ENOUGH DATA: ");
            Serial.println(nbytes);
        }
    } else { // there is no client with data
        int time_since_last_req = (current_time - last_req_time);
        if ((rc_timeout < time_since_last_req) && !is_paused) {
            pause_activities();
        }
        if (client_timeout < time_since_last_req) {
            if (last_client.connected()) {
                Serial.println("Wifi: Disconnecting client");
                last_client.stop();
            }
        }
        if (reset_timeout < time_since_last_req) {
            Serial.println("Wifi: Resetting");
            resetArduino();
        }
    }
}