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

String current_cmd = "";

void decode_cmd() {
  if (current_cmd.length() < 3) {
    return;
  }

  Serial.println("Decoding cmd");
  // Motor speed command
  if (current_cmd[0] == 'm') {
    // Parse motor number
    String nstr = current_cmd.substring(1,2);
    int n = nstr.toInt();

    if (n == 1 || n == 2) {
      // Parse motor speed & set
      String speedstr = current_cmd.substring(2);
      int speed = speedstr.toInt();
      set_motor_speed(n, speed);
    } else {
      wifi_server.println("Invalid motor number");
    }
  }
}

void tick_wifi() {
  // wait & obtain the connected client
  WiFiClient client = wifi_server.available();
  if (client) {

    // Clear input buffer if new connection
    if (!is_client_connected) {
      is_client_connected = true;
      client.flush();
      // discard whatever bytes come in at the start of connection
      while (client.available()) {
        client.read();
      }
      wifi_server.println("Welcome to the Fourth Wheel");
    }

    // While there bytes available to read from the client, store in current_cmd
    while (client.available()) {
      char ch = client.read();
      if (ch == '\n') {
        // Execute command on newline
        decode_cmd();
        current_cmd = "";
      }
      else if (ch != '\r') {
        current_cmd += ch;
      }
    }
  }
}