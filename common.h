#ifndef COMMON_H
#define COMMON_H

#include "pico/types.h"

// -------- generic --------

/*
 * Number of GPIO pins on the Raspberry Pi Pico.
 */
#define GPIO_MAX 28

#define GPIO_LED_ON_OFF 12
#define GPIO_LED_GLOW 13
#define GPIO_LED_START 14
#define GPIO_LED_STOP 15

#define GPIO_SWITCH_GLOW 17
#define GPIO_SWITCH_START 16
#define GPIO_SWITCH_STOP 18

/*
 * Relay to control the warnings panel (oil pressure, water in sail drive,
 * coolant high temperature, low battert).
 *
 * Corresponds to Yanmar's AC terminal.
 *
 * NOTE: because the panel is enabled when in STATE_IDLE, it is wired between
 * the relays Common and Normally-Closed connections (all other relays use
 * Command and Normally-Open). This means that all calls to relay_set should
 * have its bool parameter inverted.
 */
#define GPIO_RELAY_PANEL 19

/*
 * Relay to control the glow plug.
 *
 * Corresponds to Yanmar's G1 and G2 terminals (which are joined together
 * closer to the glow plug).
 */
#define GPIO_RELAY_GLOW 20

/*
 * Relay to control the starter motor.
 *
 * Corresponds to Yanmar's 17 terminal.
 */
#define GPIO_RELAY_START 21

/*
 * Relay to stop the engine.
 *
 * Corresponds to Yanmar's stop push button, connecting +12V to the Brown-White
 * cable.
 */
#define GPIO_RELAY_STOP 22

#define LED_VALUE_BRIGHT 100
#define LED_VALUE_DIM 40
#define LED_VALUE_OFF 0

#define DURATION_SUBSTATE_START_ENABLED_BRIGHT_MS 1000
#define DURATION_SUBSTATE_START_ENABLED_DIM_MS 4000

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

// -------- hardware relays --------

/*
 * Initialize a GPIO pin for control of a relay.
 *
 * `gpio` is the logical GPIO pin, e.g. 16 for physical pin 21.
 */
void relay_register(uint gpio);

/*
 * De-initialize a previously initialized GPIO pin.
 */
void relay_unregister(uint gpio);

/*
 * Engage or disengage a relay.
 *
 * `engage`: true to connect C and NO, false to connect C and NC
 */
void relay_set(uint gpio, bool engage);

#endif
