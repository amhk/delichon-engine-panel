#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "pico/time.h"

#include "common.h"

struct led_info {
  uint16_t target_value;
  uint16_t current_value;
};

static struct led_info leds[GPIO_MAX];

static repeating_timer_t repeating_timer;

static bool repeating_timer_is_active = false;

bool on_repeating_timer(__unused repeating_timer_t* rt) {
  uint count = 0;
  for (uint gpio = 0; gpio < GPIO_MAX; gpio++) {
    struct led_info* led = &leds[gpio];
    if (led->current_value > led->target_value) {
      ++count;
      pwm_set_gpio_level(gpio, --led->current_value);
    } else if (led->current_value < led->target_value) {
      ++count;
      pwm_set_gpio_level(gpio, ++led->current_value);
    }
  }

  repeating_timer_is_active = count > 0;
  return repeating_timer_is_active;
}

static inline uint16_t percent_to_led_value(uint8_t percent) {
  assert(percent <= 100);

  const uint16_t x = (((uint32_t)percent) * 255) / 100;
  return x * x;
}

void led_register(uint gpio) {
  assert(gpio < GPIO_MAX);

  gpio_set_function(gpio, GPIO_FUNC_PWM);
  const uint slice_num = pwm_gpio_to_slice_num(gpio);

  pwm_config config = pwm_get_default_config();
  pwm_init(slice_num, &config, true);

  pwm_clear_irq(slice_num);
  pwm_set_irq_enabled(slice_num, true);

  led_jump_to(gpio, 0);
}

void led_unregister(uint gpio) {
  assert(gpio < GPIO_MAX);

  struct led_info* led = &leds[gpio];
  led->target_value = 0;
  led->current_value = 0;
  pwm_set_gpio_level(gpio, 0);

  const uint slice_num = pwm_gpio_to_slice_num(gpio);
  pwm_set_irq_enabled(slice_num, false);
}

void led_jump_to(uint gpio, uint8_t percent) {
  assert(gpio < GPIO_MAX);
  assert(percent <= 100);

  uint16_t value = percent_to_led_value(percent);
  struct led_info* led = &leds[gpio];
  led->target_value = value;
  led->current_value = value;
  pwm_set_gpio_level(gpio, value);
}

void led_animate_to(uint gpio, uint8_t percent) {
  assert(gpio < GPIO_MAX);
  assert(percent <= 100);

  uint16_t value = percent_to_led_value(percent);
  struct led_info* led = &leds[gpio];
  led->target_value = value;

  if (!repeating_timer_is_active) {
    repeating_timer_is_active = true;
    add_repeating_timer_us(20, on_repeating_timer, NULL, &repeating_timer);
  }
}
