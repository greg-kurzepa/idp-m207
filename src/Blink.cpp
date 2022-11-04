/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>

#include <SPI.h>
#include <WiFiNINA.h>

// Wifi
char wifi_ssid[] = "squirrel";
char wifi_pass[] = "75/44xG1";
int wifi_status = WL_IDLE_STATUS;
WiFiServer wifi_server(23); // arg1 = server port

void setup_ap() {
  Serial.println("Starting web server for access point");

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERR: Could not communicate with Wi-Fi module");
    while (true); // STOP
  }

  Serial.println("Starting access point");

  // Access point
  // ap_status = WiFi.beginAP(ap_ssid, ap_pass);
  // if (ap_status != WL_AP_LISTENING) {
  //   Serial.println("ERR: Could not start access point");
  //   while (true); // STOP
  // }
  // // wait for AP connection
  // delay(5);
  // ap_server.begin();

  while (wifi_status != WL_CONNECTED) {
    Serial.print("Wifi: Connecting to SSID: ");
    Serial.println(wifi_ssid);
    wifi_status = WiFi.begin(wifi_ssid, wifi_pass);
    delay(3000); // wait for connection
  }
  wifi_server.begin();

  IPAddress ip = WiFi.localIP();
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
    // if (!alreadyConnected) {
    //   // clead out the input buffer:
    //   client.flush();
    //   Serial.println("We have a new client");
    //   client.println("Hello, client!");
    //   alreadyConnected = true;
    // }
    if (client.available()) {
      // then there bytes available to read from the client,
      char thisChar = client.read();
      // echo
      wifi_server.write(thisChar);
      // echo to server
      Serial.write(thisChar);
    }
  }
}

// void tick_ap() {
//   if (wifi_status != WiFi.status()) {
//     // When status changed
//     wifi_status = WiFi.status();
//     if (wifi_status == WL_AP_CONNECTED) {
//       Serial.println("AP: Device connected");
//     } else {
//       Serial.println("AP: Device disconnected, now listening");
//     }
//   }

//   WiFiClient client = wifi_server.available();
//   if (client) {
//     Serial.println("AP: New client");
//     String currentLine = "";
//     while (client.connected()) {
//       if (client.available()) {
//         // then there bytes available to read from the client,
//         char c = client.read(); // byte
//         Serial.write(c);
//         if (c == '\n') {
//           if (currentLine.length() == 0) {
//             // HTTP response
//             client.println("HTTP/1.1 200 OK");
//             client.println("Content-type:text/html");
//             client.println();

//             // Content
//             client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
//             client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");

//             client.println();
//             break;
//           }
//           else {
//             currentLine = "";
//           }
//         }
//         else if (c != '\r') {
//           currentLine += c;
//         }

//         // Actions
//         if (currentLine.endsWith("GET /H")) {
//           digitalWrite(LED_BUILTIN, HIGH);
//         }
//         if (currentLine.endsWith("GET /L")) {
//           digitalWrite(LED_BUILTIN, LOW);
//         }
//       }
//     }
//     // close the connection:
//     client.stop();
//     Serial.println("client disconnected");
//   }
// }

void loop() {
  // Serial.println("Hello World");
  tick_wifi();
  // delay(2000);
}
