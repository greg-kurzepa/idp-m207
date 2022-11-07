/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include "motor.hpp"

// placeholder function, will determine speed robot should turn at to return back to right path
int get_turn_speed() {return 1;}

void adjust_motor(int mode) {
    switch (mode)
    {
    case 1: // mode 1: 3 line sensors in a row
      get_turn_speed();
      break;
    }
}

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
}

void loop() {
  tick_wifi();
  get_follower_readings();
  adjust_motor(1);
}