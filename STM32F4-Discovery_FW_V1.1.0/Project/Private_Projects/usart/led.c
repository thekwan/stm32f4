#include "stm32f4xx.h"
#include "led.h"

uint16_t      LED_GPIO_CLK[LEDn]  = {LED1_GPIO_CLK , LED2_GPIO_CLK };
uint16_t      LED_GPIO_PIN[LEDn]  = {LED1_GPIO_PIN , LED2_GPIO_PIN };
GPIO_TypeDef* LED_GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT};

/*
 * LED Hardware Initialization
 * Initialize GPIO pin
 */
int bsp_led_hw_init( int led )
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	/* Enable Clock */
	RCC_AHB1PeriphClockCmd(LED_GPIO_CLK[led], ENABLE);

	/* GPIO Pin set */
	GPIO_InitStructure.GPIO_Pin    = LED_GPIO_PIN[led];
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
	GPIO_Init(LED_GPIO_PORT[led], &GPIO_InitStructure);

	bsp_led_off( led );

	return 0;
}

/*
 * Turn on LED
 */
int bsp_led_on( int led )
{
	((GPIO_TypeDef *) LED_GPIO_PORT[led])->ODR |= LED_GPIO_PIN[led];

	return 0;
}

/*
 * Turn off LED
 */
int bsp_led_off( int led )
{
	((GPIO_TypeDef *) LED_GPIO_PORT[led])->ODR &= ~LED_GPIO_PIN[led];

	return 0;
}

/*
 * Toggle LED
 */
int bsp_led_toggle( int led )
{
	((GPIO_TypeDef *) LED_GPIO_PORT[led])->ODR = LED_GPIO_PORT[led]->ODR ^ LED_GPIO_PIN[led];

	return 0;
}
