#include "motor.hpp"

extern int prev_millis {0};
extern int curr_millis {0};

void update_time() {
  prev_millis = curr_millis;
  curr_millis = millis();
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

  setup_ultrasonic();
}

void loop() {
  update_time();
  pulse_motion_led();
  tick_wifi();
}