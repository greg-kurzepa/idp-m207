/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>

#include <SPI.h>
#include <WiFiNINA.h>
// #include <wifi_secrets.h>

// Wifi
// char wifi_ssid[] = WifiSecrets::SSID;
// char wifi_pass[] = WifiSecrets::PASS;
char wifi_ssid[] = "squirrel";
char wifi_pass[] = "75/44xG1";

int wifi_status = WL_IDLE_STATUS;
WiFiServer wifi_server(23); // arg1 = server port
bool is_client_connected = false;

void setup_ap() {
  Serial.println("Starting web server for access point");

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERR: Could not communicate with Wi-Fi module");
    while (true); // STOP
  }

  while (wifi_status != WL_CONNECTED) {
    Serial.print("Wifi: Connecting to SSID: ");
    Serial.println(wifi_ssid);
    wifi_status = WiFi.begin(wifi_ssid, wifi_pass);
    delay(3000); // wait for connection
  }
  wifi_server.begin();

  IPAddress ip = WiFi.localIP();
  Serial.println("Wifi: Connected from IP address:");
  Serial.println(ip);

}

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect; necessary for native USB port
  }

  pinMode(LED_BUILTIN, OUTPUT);

  setup_ap();
}

void tick_wifi() {
  // wait for a new client to connect
  WiFiClient client = wifi_server.available();
  if (client) {
    // Clear input buffer if new connection
    if (!is_client_connected) {
      client.flush();
      wifi_server.println("Welcome to the Fourth Wheel");
      is_client_connected = true;
    }

    if (client.available()) {
      // Then: there bytes available to read from the client,
      char ch = client.read();
      if (ch == 'f') {
        wifi_server.println("Respects paid");
      } else if (ch == '\r' || ch == '\n') {}
      else {
        wifi_server.print("Unknown command: ");
        wifi_server.write(ch);
        wifi_server.println();
      }
    }
  }
}

void loop() {
  tick_wifi();
}
