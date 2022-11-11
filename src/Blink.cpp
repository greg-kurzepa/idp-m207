/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include "motor.hpp"

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect; necessary for native USB port
  }

  pinMode(LED_BUILTIN, OUTPUT);

  setup_motors();
  setup_follower();
  setup_wifi();

  setup_ultrasonic();
}

void loop() {
  tick_wifi();
  get_follower_readings();
}
