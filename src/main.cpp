#include "motor.hpp"
#include <MemoryFree.h>

extern long prev_millis {0};
extern long curr_millis {0};

extern int max_looptime {0};

void update_time() {
  prev_millis = curr_millis;
  curr_millis = millis();
  if (curr_millis - prev_millis > max_looptime) {
    max_looptime = curr_millis - prev_millis;
    // Serial.println(max_looptime);
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
  setup_motion_led();
  setup_block_leds();
  setup_grabber();
  setup_wifi();

  setup_ultrasonic();
  
}

void loop() {
  update_time();
  update_grabber();
  update_motion_led();
  update_block_leds();
  tick_wifi();
}