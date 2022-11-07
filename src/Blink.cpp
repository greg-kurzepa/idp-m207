/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <wifi.cpp>
#include <motor.hpp>

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect; necessary for native USB port
  }

  pinMode(LED_BUILTIN, OUTPUT);

  setup_wifi();
  setup_motors();
  setup_follower();
  set_motor_speed(1,0);
  set_motor_speed(2,0);
}

void loop() {
  tick_wifi();
  get_follower_readings();
}
