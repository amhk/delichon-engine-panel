#include "hardware/gpio.h"

#include "common.h"

void relay_register(uint gpio) {
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_OUT);
  relay_set(gpio, false);
}

void relay_unregister(uint gpio) {
  relay_set(gpio, false);
}

void relay_set(uint gpio, bool engage) {
  gpio_put(gpio, engage ? 1 : 0);
}
