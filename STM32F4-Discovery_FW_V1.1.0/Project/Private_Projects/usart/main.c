

//#define USE_STDPERIPH_DRIVER
#include "stm32f4xx.h"

#include "usart.h"
#include "led.h"
 

//Quick hack, approximately 1ms delay
void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}

int main(void)
{
	char receive_string[128];
	uart4_hw_init( );

	bsp_led_hw_init(LED1);
	bsp_led_hw_init(LED2);

	bsp_led_on(LED1);
	bsp_led_on(LED2);

	for (;;) {
#if 0	/* Simplest basic test code */
		ms_delay(1000);
		SendMessage( "Hellow World!\n\r" );
#else
		SendMessage( "#> ", NONE );
		ReceiveMessage( receive_string, 128);

		bsp_led_toggle(LED2);
		bsp_led_off(LED1);

		if( receive_string[0] != 0x0 )
			SendMessage( receive_string, NEWLINE );
#endif
	}

	return 0;
}
