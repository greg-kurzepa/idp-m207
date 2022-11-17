#include "motor.hpp"
#include <MemoryFree.h>

extern int prev_millis {0};
extern int curr_millis {0};

extern int max_looptime {0};

void update_time() {
  prev_millis = curr_millis;
  curr_millis = millis();
  if (curr_millis - prev_millis > max_looptime) {
    max_looptime = curr_millis - prev_millis;
    Serial.println(max_looptime);
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
  setup_leds();
  setup_wifi();

  setup_ultrasonic();
}

void loop() {
  update_time();
  update_leds();
  tick_wifi();
  //Serial.println(freeMemory(), DEC);
}