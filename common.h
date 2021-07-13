#ifndef COMMON_H
#define COMMON_H

#include "pico/types.h"

/*
 * Number of GPIO pins on the Raspberry Pi Pico.
 */
#define GPIO_MAX 28

// -------- hardware buttons --------

enum button_event {
  BUTTON_PRESSED,
  BUTTON_RELEASED,
};

/*
 * Callback function called when a hardware button is pressed or released.
 */
typedef void (*button_cb_t)(enum button_event);

/*
 * Register a callback for a hardware button. The framework will handle
 * debouncing on behalf of the caller.
 *
 * The button is assumed to be wired to a) 3.3V and b) the GPIO pin. The GPIO
 * pin should preferably have a physical pull down resistor to ground, but the
 * framework enables Pico's built-in pull down resistor.
 *
 * `gpio` is the logical GPIO pin, e.g. 16 for physical pin 21.
 */
void button_register(uint gpio, button_cb_t callback);

/*
 * Unregister a previously registered callback for a hardware button.
 */
void button_unregister(uint gpio);

// -------- hardware LEDs --------

/*
 * Initialize a GPIO pin for PWM control of LED.
 *
 * `gpio` is the logical GPIO pin, e.g. 16 for physical pin 21.
 */
void led_register(uint gpio);

/*
 * De-initialize a previously initialized GPIO pin.
 */
void led_unregister(uint gpio);

/*
 * Immediately set the brightness of the LED.
 *
 * `percent` should be between 0 and 100, inclusive.
 */
void led_jump_to(uint gpio, uint8_t percent);

/*
 * Change the brightness of the LED by animating from its current value to the
 * given value.
 *
 * `percent` should be between 0 and 100, inclusive.
 */
void led_animate_to(uint gpio, uint8_t percent);

#endif
