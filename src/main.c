/*
 * Author: Alberto Venzo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#define SLEEP_TIME_MS 400

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define STACKSIZE 1024
#define PRIORITY_LOW 7

#define LED_LIGHTING_UP (1)
#define LED_FADING_DOWN (0)

//static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_orange = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
static const struct pwm_dt_spec pwm_led_green = PWM_DT_SPEC_GET(DT_ALIAS(stm32f4pwm0));

int stm32f4_gpio_pin_configure(const struct gpio_dt_spec *led)
{
	int ret;
	if (!gpio_is_ready_dt(led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

  return ret;
}

int main(void)
{
	bool led_state = true;
	int ret[4];
  int i;

  //stm32f4_gpio_pin_configure(&led_green);
  //stm32f4_gpio_pin_configure(&led_orange);
  //stm32f4_gpio_pin_configure(&led_red);
  //stm32f4_gpio_pin_configure(&led_blue);

	while (1) {

		//ret[0] = gpio_pin_toggle_dt(&led_green);
		//ret[1] = gpio_pin_toggle_dt(&led_orange);
		//ret[2] = gpio_pin_toggle_dt(&led_red);
		//ret[3] = gpio_pin_toggle_dt(&led_blue);

    //for(i = 1; i < 4; i++) {
		  //if (ret[i] < 0) {
			//return 0;
		  //}
    //}

		//led_state = !led_state;
		//printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}

/* General function to pass to a thread */
void stm32f4_led_thread(const struct gpio_dt_spec *led, uint32_t thread_sleep_ms)
{
  int ret;

  stm32f4_gpio_pin_configure(led);

	while (1) {

    ret = gpio_pin_toggle_dt(led);
    if (ret < 0) {
      return;
    }
    k_msleep(thread_sleep_ms);
  }

}

/* General pwm thread function */
void stm32f4_pwm_thread(const struct pwm_dt_spec *pwm_led, uint32_t thread_sleep_ms)
{
  uint32_t max_period;
  uint32_t period;
  uint32_t pulse_width;
  uint8_t dir = 0U;
  int ret;

  if (!pwm_is_ready_dt(pwm_led)) {
    return;
  }

  max_period = 100000U;
  pwm_set_dt(pwm_led, max_period, max_period / 2U);

  // changed the sample code for PWM because it was not so much clear to me
  period = max_period;
  pulse_width = period / 2U;
  while (1) {
    ret = pwm_set_dt(pwm_led, period, pulse_width);
    if (ret) {
      return;
    }
  if(dir == LED_LIGHTING_UP) {
    pulse_width += period/50;
    if(pulse_width > (period - 1000)) {
      dir = LED_FADING_DOWN;
    }
  } else if(dir == LED_FADING_DOWN) {
    pulse_width -= period/50;
    if(pulse_width < 1000){
      dir = LED_LIGHTING_UP;
    }
  }
    k_msleep(thread_sleep_ms);
  }

  return;
}

// Thread functions
/*
void stm32f4_green_led_thread(void)
{
  stm32f4_led_thread(&led_green, 100);
}
*/

void stm32f4_green_pwm_led_thread(void)
{
   stm32f4_pwm_thread(&pwm_led_green, 25);
}

void stm32f4_orange_led_thread(void)
{
  stm32f4_led_thread(&led_orange, 200);
}

void stm32f4_red_led_thread(void)
{
  stm32f4_led_thread(&led_red, 300);
}

void stm32f4_blue_led_thread(void)
{
  stm32f4_led_thread(&led_blue, 400);
}

//K_THREAD_DEFINE(blink_green_id, STACKSIZE, stm32f4_green_led_thread, NULL, NULL, NULL,
    //PRIORITY_LOW, 0, 0);

K_THREAD_DEFINE(blink_green_id, 2048, stm32f4_green_pwm_led_thread, NULL, NULL, NULL,
    PRIORITY_LOW, 0, 0);
K_THREAD_DEFINE(blink_orange_id, STACKSIZE, stm32f4_orange_led_thread, NULL, NULL, NULL,
    PRIORITY_LOW, 0, 0);
K_THREAD_DEFINE(blink_red_id, STACKSIZE, stm32f4_red_led_thread, NULL, NULL, NULL,
    PRIORITY_LOW, 0, 0);
K_THREAD_DEFINE(blink_blue_id, STACKSIZE, stm32f4_blue_led_thread, NULL, NULL, NULL,
    PRIORITY_LOW, 0, 0);
