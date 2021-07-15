#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "common.h"

static enum state {
  STATE_IDLE,
  STATE_GLOW,
  STATE_START,
  STATE_STOP,
} state;

static enum substate {
  SUBSTATE_START_ENABLED_BRIGHT,
  SUBSTATE_START_ENABLED_DIM,
  SUBSTATE_START_DISABLED,
} substate = SUBSTATE_START_ENABLED_BRIGHT;

static void goto_state_idle() {
  printf("goto_state_idle\n");
  switch (substate) {
  case SUBSTATE_START_ENABLED_BRIGHT:
    led_jump_to(GPIO_LED_ON_OFF, LED_VALUE_BRIGHT);
    led_jump_to(GPIO_LED_GLOW, LED_VALUE_BRIGHT);
    led_jump_to(GPIO_LED_START, LED_VALUE_BRIGHT);
    led_jump_to(GPIO_LED_STOP, LED_VALUE_BRIGHT);
    break;
  case SUBSTATE_START_ENABLED_DIM:
    if (state == STATE_IDLE) {
      led_animate_to(GPIO_LED_ON_OFF, LED_VALUE_DIM);
      led_animate_to(GPIO_LED_GLOW, LED_VALUE_DIM);
      led_animate_to(GPIO_LED_START, LED_VALUE_DIM);
      led_animate_to(GPIO_LED_STOP, LED_VALUE_DIM);
    } else {
      led_jump_to(GPIO_LED_ON_OFF, LED_VALUE_DIM);
      led_jump_to(GPIO_LED_GLOW, LED_VALUE_DIM);
      led_jump_to(GPIO_LED_START, LED_VALUE_DIM);
      led_jump_to(GPIO_LED_STOP, LED_VALUE_DIM);
    }
    break;
  case SUBSTATE_START_DISABLED:
    if (state == STATE_IDLE) {
      led_animate_to(GPIO_LED_ON_OFF, LED_VALUE_DIM);
      led_animate_to(GPIO_LED_GLOW, LED_VALUE_OFF);
      led_animate_to(GPIO_LED_START, LED_VALUE_OFF);
      led_animate_to(GPIO_LED_STOP, LED_VALUE_DIM);
    } else {
      led_jump_to(GPIO_LED_ON_OFF, LED_VALUE_DIM);
      led_jump_to(GPIO_LED_GLOW, LED_VALUE_OFF);
      led_jump_to(GPIO_LED_START, LED_VALUE_OFF);
      led_jump_to(GPIO_LED_STOP, LED_VALUE_DIM);
    }
    break;
  }
  state = STATE_IDLE;
}

static void goto_state_start() {
  printf("goto_state_start\n");
  led_jump_to(GPIO_LED_ON_OFF, LED_VALUE_OFF);
  led_jump_to(GPIO_LED_GLOW, LED_VALUE_OFF);
  led_jump_to(GPIO_LED_START, LED_VALUE_BRIGHT);
  led_jump_to(GPIO_LED_STOP, LED_VALUE_OFF);
  state = STATE_START;
}

static void goto_state_glow() {
  printf("goto_state_glow\n");
  led_jump_to(GPIO_LED_ON_OFF, LED_VALUE_OFF);
  led_jump_to(GPIO_LED_GLOW, LED_VALUE_BRIGHT);
  led_jump_to(GPIO_LED_START, LED_VALUE_OFF);
  led_jump_to(GPIO_LED_STOP, LED_VALUE_OFF);
  state = STATE_GLOW;
}

static void goto_state_stop() {
  printf("goto_state_stop\n");
  led_jump_to(GPIO_LED_ON_OFF,
              substate == SUBSTATE_START_ENABLED_BRIGHT ? LED_VALUE_BRIGHT : LED_VALUE_DIM);
  led_jump_to(GPIO_LED_GLOW, LED_VALUE_OFF);
  led_jump_to(GPIO_LED_START, LED_VALUE_OFF);
  led_jump_to(GPIO_LED_STOP, LED_VALUE_BRIGHT);
  state = STATE_STOP;
}

static void on_button_start(enum button_event event) {
  if (event == BUTTON_PRESSED && state == STATE_IDLE && substate != SUBSTATE_START_DISABLED) {
    goto_state_start();
  } else if (event == BUTTON_RELEASED && state == STATE_START) {
    goto_state_idle();
  }
}

static void on_button_glow(enum button_event event) {
  if (event == BUTTON_PRESSED && state == STATE_IDLE && substate != SUBSTATE_START_DISABLED) {
    goto_state_glow();
  } else if (event == BUTTON_RELEASED && state == STATE_GLOW) {
    goto_state_idle();
  }
}

static void on_button_stop(enum button_event event) {
  if (event == BUTTON_PRESSED && state == STATE_IDLE) {
    goto_state_stop();
  } else if (event == BUTTON_RELEASED && state == STATE_STOP) {
    goto_state_idle();
  }
}

static int64_t on_alarm(__unused alarm_id_t id, void* user_data) {
  printf("on_alarm user_data=%d\n", (int)user_data);
  substate = (int)user_data;
  if (state == STATE_IDLE) {
    goto_state_idle(); // update LEDs
  }
  return 0;
}

int main() {
  stdio_init_all();

  button_register(GPIO_SWITCH_GLOW, on_button_glow);
  button_register(GPIO_SWITCH_START, on_button_start);
  button_register(GPIO_SWITCH_STOP, on_button_stop);

  led_register(GPIO_LED_ON_OFF);
  led_register(GPIO_LED_GLOW);
  led_register(GPIO_LED_START);
  led_register(GPIO_LED_STOP);

  add_alarm_in_ms(DURATION_SUBSTATE_START_ENABLED_BRIGHT_MS, on_alarm,
                  (void*)SUBSTATE_START_ENABLED_DIM, true);
  add_alarm_in_ms(DURATION_SUBSTATE_START_ENABLED_BRIGHT_MS +
                      DURATION_SUBSTATE_START_ENABLED_DIM_MS,
                  on_alarm, (void*)SUBSTATE_START_DISABLED, true);

  goto_state_idle();

  while (true) {
    tight_loop_contents();
  }
}
