#include "core.hpp"
#include <MemoryFree.h>

// Timestamp of the moment before the main loop cycle
extern long cur_cycle_time = 0;
// Same as cur_cycle_time, but for the previous cycle
extern long prev_cycle_time = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect; necessary for native USB port
  }

  cur_cycle_time = millis();
  setup_motors();
  setup_followers();
  setup_ultrasonic();
  setup_motion_led();
  setup_block_leds();
  setup_wifi();
  prev_cycle_time = cur_cycle_time;
}

void loop() {
  cur_cycle_time = millis();
  update_grabber();
  update_motion_led();
  update_followers();
  
  update_wifi();
  prev_cycle_time = cur_cycle_time;
}