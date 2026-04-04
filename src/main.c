/*
 * Author: Alberto Venzo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define SLEEP_TIME_MS 400

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define STACKSIZE 1024
#define PRIORITY_LOW 7

static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_orange = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

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

  stm32f4_gpio_pin_configure(&led_green);
  stm32f4_gpio_pin_configure(&led_orange);
  stm32f4_gpio_pin_configure(&led_red);
  stm32f4_gpio_pin_configure(&led_blue);

	while (1) {

		ret[0] = gpio_pin_toggle_dt(&led_green);
		ret[1] = gpio_pin_toggle_dt(&led_orange);
		ret[2] = gpio_pin_toggle_dt(&led_red);
		ret[3] = gpio_pin_toggle_dt(&led_blue);

    for(i = 0; i < 4; i++) {
		  if (ret[i] < 0) {
			return 0;
		  }
    }

		led_state = !led_state;
		printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}

/**
K_THREAD_DEFINE(blink_green_id, STACKSIZE, blink0, NULL, NULL, NULL,
    PRIORITY_LOW, 0, 0);
**/
