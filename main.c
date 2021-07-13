#include <stdio.h>

#include "pico/stdlib.h"

#include "common.h"

static void on_button_one(enum button_event event) {
  printf("button one: %d\n", event);
}

static void on_button_two(enum button_event event) {
  printf("button two: %d\n", event);
}

static void on_button_three(enum button_event event) {
  if (event == BUTTON_PRESSED) {
    printf("all buttons unregistered\n");
    button_unregister(16);
    button_unregister(17);
    button_unregister(18);
  }
}

int main() {
  stdio_init_all();
  button_register(16, on_button_one);
  button_register(17, on_button_two);
  button_register(18, on_button_three);

  while (true) {
    tight_loop_contents();
  }
}
