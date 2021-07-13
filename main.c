#include <stdio.h>

#include "pico/stdlib.h"

#include "common.h"

static void on_button_one(enum button_event event) {
  if (event == BUTTON_PRESSED) {
    led_jump_to(14, 0);
    led_jump_to(15, 0);
  }
}

static void on_button_two(enum button_event event) {
  if (event == BUTTON_PRESSED) {
    led_jump_to(14, 100);
    led_jump_to(15, 100);
  }
}

static void on_button_three(enum button_event event) {
  if (event == BUTTON_PRESSED) {
    led_animate_to(14, 50);
    led_animate_to(15, 50);
  }
}

int main() {
  stdio_init_all();

  button_register(16, on_button_one);
  button_register(17, on_button_two);
  button_register(18, on_button_three);

  led_register(14);
  led_register(15);
  led_jump_to(14, 100);
  led_jump_to(15, 100);

  while (true) {
    tight_loop_contents();
  }
}
