#include "hardware/gpio.h"
#include "pico/time.h"

#include "common.h"

#define GPIO_MAX 28

struct button_info {
  button_cb_t callback;
  absolute_time_t timestamp;
  enum button_event state;
};

static struct button_info buttons[GPIO_MAX];

static void internal_callback(uint gpio, uint32_t events) {
  assert(gpio < GPIO_MAX);

  struct button_info* info = &buttons[gpio];
  if (info == NULL) {
    return;
  }

  const absolute_time_t now = get_absolute_time();
  if (absolute_time_diff_us(info->timestamp, now) < 20 * 1000) {
    return;
  }
  info->timestamp = now;

  if (info->state == BUTTON_RELEASED && (events & GPIO_IRQ_EDGE_RISE) != 0) {
    info->callback(info->state = BUTTON_PRESSED);
  } else if (info->state == BUTTON_PRESSED && (events & GPIO_IRQ_EDGE_FALL) != 0) {
    info->callback(info->state = BUTTON_RELEASED);
  }
}

void button_register(uint gpio, button_cb_t callback) {
  assert(gpio < GPIO_MAX);
  assert(callback != NULL);

  struct button_info* info = &buttons[gpio];
  info->callback = callback;
  info->timestamp = nil_time;
  info->state = BUTTON_RELEASED;

  gpio_pull_down(gpio);
  gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true,
                                     &internal_callback);
}

void button_unregister(uint gpio) {
  assert(gpio < GPIO_MAX);

  struct button_info* info = &buttons[gpio];
  info->callback = NULL;
}
