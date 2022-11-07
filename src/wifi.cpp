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

void decode_motor_speed_cmd(String str) {
    // Parse motor number
    // 49 = ascii 1; 50 = ascii 2
    int n = (int)str[0] - 48;

    if (n == 1 || n == 2) {
      // Speed is two bytes - magnitude (40+[0,64]) + (40+[0,3])
      // Weird things happen when using full range of char, so use limited range
      // and use offset of 40
      int speed = str[1] - 40;
      speed += 64 * (str[2] - 40);
      set_motor_speed(n, speed);
    } else {
      wifi_server.println("Invalid motor number");
    }
}

// Takes a sequence of commands,
// executes the first command,
// and returns remaining unprocessed commands 
String decode_cmd(String str) {
  int strlen = str.length();
  if (strlen == 0) {
    return "";
  }

  char cmd_ch = str[0];
  String rest = "";
  switch (cmd_ch)
  {
  case 'm': // Set motor speed
    int cmd_len = 4;
    if (strlen >= cmd_len) {
      rest = str.substring(cmd_len);
      decode_motor_speed_cmd(str.substring(1,cmd_len));
    }
    break;
  
  default:
    break;
  }
  return rest;
}

String current_cmd = "";

void decode_cmds() {
  Serial.println("Decoding cmds");
  // Motor speed command
  String str = current_cmd;
  while (str.length() > 0) {
    str = decode_cmd(str);
  }
  current_cmd = "";
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

    // ~~While there bytes available to read from the client, store in current_cmd
    while (client.available()) {
      char ch = client.read();
      // if (ch == '\n') {
      //   // Execute command on newline
      //   decode_cmds();
      // }
      // else if (ch != '\r') {
        current_cmd += ch;
      // }
    }
    decode_cmds();
  }
}