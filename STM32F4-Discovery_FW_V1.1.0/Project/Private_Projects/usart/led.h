#include "stm32f4xx.h"

#ifndef _LED_H_
#define _LED_H_

#define LEDn	2

#define LED1    0
#define LED2    1

#define LED1_GPIO_PIN                    GPIO_Pin_14
#define LED1_GPIO_PORT                   GPIOG
#define LED1_GPIO_CLK                    RCC_AHB1Periph_GPIOG  
  
#define LED2_GPIO_PIN                    GPIO_Pin_13
#define LED2_GPIO_PORT                   GPIOG
#define LED2_GPIO_CLK                    RCC_AHB1Periph_GPIOG  

typedef enum {
  OFF = 0,
  ON  = 1
} LED_STATE;

int bsp_led_hw_init(int);
int bsp_led_on(int);
int bsp_led_off(int);
int bsp_led_toggle(int);

#endif
