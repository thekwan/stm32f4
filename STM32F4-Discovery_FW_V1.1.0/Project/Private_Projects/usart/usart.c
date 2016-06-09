#include "stm32f4xx.h"
#include "usart.h"

#define UART_MSG_BUFF_SIZE	128

// TX and RX software buffer, 
// but RX routine is not implemented yet (2016.2.14)
char UART_RX_BUFF[ UART_MSG_BUFF_SIZE ];
char UART_TX_BUFF[ UART_MSG_BUFF_SIZE ];

int  UART_RX_BUFF_sptr = 0;
int  UART_RX_BUFF_eptr = 0;
int  UART_TX_BUFF_sptr = 0;
int  UART_TX_BUFF_eptr = 0;

// The variable below is to indicate whether TXE interrupt is enabled or disabled.
// The TXE interrupt is only enabled whenever there are data to be transmitted.
// (when there is no data, then we disabled this interrupt by software)
int  enabled_IT_TXE = 0;

void uart4_hw_init( void )
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	// UART4	-----------------------------------------
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	// enable the clock to UART4 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	// enable the UART TX/RX port
    __asm("dsb");         // stall instruction pipeline, until instruction completes, as
                          //    per Errata 2.1.13, "Delay after an RCC peripheral clock enabling"

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig( GPIOA, GPIO_PinSource0, GPIO_AF_UART4 );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource1, GPIO_AF_UART4 );

	USART_InitTypeDef	USART_InitStructure;
	USART_StructInit( &USART_InitStructure );
	USART_Init( UART4, &USART_InitStructure );

	USART_Cmd(UART4, ENABLE);


	// NVIC  	-----------------------------------------
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init( &NVIC_InitStructure );

	//printString("H/W configuration is completed !\n\r");
	SendMessage("DEBUG: UART4 initialization is completed !\n\r");

	return;
}


int EnQueue( char *buff, int *sptr, int *eptr, char msg )
{
	int next_eptr = *eptr + 1;
	if( next_eptr >= UART_MSG_BUFF_SIZE ) {
		next_eptr -= UART_MSG_BUFF_SIZE;
	}

	if( next_eptr == *sptr ) {
		return 0;	// Full
	}

	buff[ *eptr ] = msg;

	*eptr = next_eptr;

	return 1;	// Not full
}

int DeQueue( char *buff, int *sptr, int *eptr, char *data )
{
	if( *sptr == *eptr ) {
		return 0;	// Empty
	}

	*data = buff[ (*sptr)++ ];

	if( *sptr >= UART_MSG_BUFF_SIZE ) {
		*sptr -= UART_MSG_BUFF_SIZE;
	}

	return 1;	// Not empty
}

void SendMessage( char *msg )
{
	while( *msg != 0x0 ) {
		while( EnQueue( UART_TX_BUFF, &UART_TX_BUFF_sptr , &UART_TX_BUFF_eptr, *msg ) != 1 );
		msg++;
		if( enabled_IT_TXE == 0 ) {
			USART_ITConfig( UART4, USART_IT_TXE, ENABLE );
			enabled_IT_TXE = 1;
		}
	}

	return;
}

// The function below is not implemented yet.
void ReceiveMessage( char *msg )
{
	return;
}

void UART4_IRQHandler( void ) 
{
	char data;

	if( USART_GetITStatus( UART4, USART_IT_TXE ) != RESET ) {	// TXE interrupt processing routine
		if( DeQueue( UART_TX_BUFF, &UART_TX_BUFF_sptr, &UART_TX_BUFF_eptr, &data ) == 1 ) {
			// if there is still remaining data to be transmitted, then only send it and wait.
			USART_SendData( UART4, data );
		}
		else {
			// if there is no remaining data to be tx, then disable the interrupt TXE of USART.
			USART_ITConfig( UART4, USART_IT_TXE, DISABLE );
			enabled_IT_TXE = 0;
		}
	}
	
}


