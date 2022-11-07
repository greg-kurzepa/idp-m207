/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
#include <SPI.h>

#include <wifi.cpp>
#include <motor.hpp>

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect; necessary for native USB port
  }

  pinMode(LED_BUILTIN, OUTPUT);

  setup_motors();
  // set_motor_speed(1,50);
  // set_motor_speed(2,50);

  setup_wifi();
}

void loop() {
  tick_wifi();
}
