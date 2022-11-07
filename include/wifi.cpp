
#include <SPI.h>
#include <WiFiNINA.h>

#include <wifi_secrets.cpp>
#include <motor.hpp>

int wifi_status = WL_IDLE_STATUS;
WiFiServer wifi_server(23); // arg1 = server port
bool is_client_connected = false;

void setup_wifi() {
  Serial.println("Wifi: Starting server");

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERR: Could not communicate with Wi-Fi module");
    while (true); // STOP
  }

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
  // wait for a new client to connect
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
    else if (client.available()) {
      // Then: there bytes available to read from the client,
      char ch = client.read();
      if (ch == 'm') {
        String nstr = "";
        nstr += (char)client.read();
        int n = nstr.toInt();
        Serial.print("Motor number: ");
        Serial.println(n);
        if (n == 1 || n == 2) {
          String speedstr = "";
          while (client.available()) {
            char ch = client.read();
            if (ch != '\r' && ch != '\n') {
              speedstr += ch;
            }
          }
          int speed = speedstr.toInt();
          Serial.print("Motor speed requested: ");
          Serial.println(speed);
          set_motor_speed(n, speed);
        } else {
          wifi_server.println("Invalid motor number");
        }
      }
      wifi_server.println("Motor speed set");
    }
  }
}