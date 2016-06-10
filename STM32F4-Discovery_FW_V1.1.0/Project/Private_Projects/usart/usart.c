#include "stm32f4xx.h"
#include "usart.h"

UartMsgBuffer UartTxBuffer;
UartMsgBuffer UartRxBuffer;

static void UartBufferClear( UartMsgBuffer *buffer );
static UartBuffStat EnQueue( UartMsgBuffer *buffer, char msg );
static UartBuffStat DeQueue( UartMsgBuffer *buffer, char *data );


// The variable below is to indicate whether TXE interrupt is enabled or disabled.
// The TXE interrupt is only enabled whenever there are data to be transmitted.
// (when there is no data, then we disabled this interrupt by software)
int  enabled_IT_TXE = 0;

int  overflow_UartRxBuffer = 0;

void uart4_hw_init( void )
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	// UART4	-----------------------------------------
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	// enable the clock to UART4 
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	// enable the UART TX/RX port
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	// enable the clock to UART4 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	// enable the UART TX/RX port
    __asm("dsb");         // stall instruction pipeline, until instruction completes, as
                          //    per Errata 2.1.13, "Delay after an RCC peripheral clock enabling"

	//GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//GPIO_PinAFConfig( GPIOA, GPIO_PinSource0, GPIO_AF_UART4 );
	//GPIO_PinAFConfig( GPIOA, GPIO_PinSource1, GPIO_AF_UART4 );
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource10, GPIO_AF_UART4 );
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource11, GPIO_AF_UART4 );

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

	UartBufferClear( &UartTxBuffer );
	UartBufferClear( &UartRxBuffer );

	USART_ITConfig( UART4, USART_IT_RXNE, ENABLE );

	SendMessage("DEBUG: UART4 initialization is completed !", NEWLINE);

	return;
}

void SendMessage( char *msg , NewLineFlag new_line_flag )
{
	while( *msg != 0x0 ) {
		while( EnQueue( &UartTxBuffer , *msg ) == FULL );
		msg++;
		if( enabled_IT_TXE == 0 ) {
			USART_ITConfig( UART4, USART_IT_TXE, ENABLE );
			enabled_IT_TXE = 1;
		}
	}

	if( new_line_flag == NEWLINE ) {
		while( EnQueue( &UartTxBuffer , 0xA ) == FULL );
		while( EnQueue( &UartTxBuffer , 0xD ) == FULL );
	}

	return;
}

// The function below is not implemented yet.
void ReceiveMessage( char *msg , int size )
{
	do {
		while( DeQueue( &UartRxBuffer , msg ) == EMPTY );
		msg++;
	} while( *(msg-1) != 0xD && (--size) > 0 );

	*(msg-1) = 0x0;

	return;
}

void UART4_IRQHandler( void ) 
{
	char data;

	if( USART_GetITStatus( UART4, USART_IT_TXE ) != RESET ) {	// TXE interrupt processing routine
		if( DeQueue( &UartTxBuffer , &data ) != EMPTY ) {
			// if there is still remaining data to be transmitted, then only send it and wait.
			USART_SendData( UART4, data );
		}
		else {
			// if there is no remaining data to be tx, then disable the interrupt TXE of USART.
			USART_ITConfig( UART4, USART_IT_TXE, DISABLE );
			enabled_IT_TXE = 0;
		}
	}
	else if( USART_GetITStatus( UART4, USART_IT_RXNE ) != RESET ) {	// RXNE interrupt processing routine
		uint16_t data = USART_ReceiveData( UART4 );

		if( EnQueue( &UartRxBuffer , data ) == FULL ) {
			// Rx buffer is full.
			overflow_UartRxBuffer = 1;
		}
	}

	return;
}

static void UartBufferClear( UartMsgBuffer *buffer )
{
	int i;

	for( i = 0 ; i < UART_MSG_BUFF_SIZE ; i++ )
		buffer->queue[i] = 0;

	buffer->start_pointer = 0;
	buffer->end_pointer   = 0;
	buffer->count         = 0;

	return;
}

static UartBuffStat EnQueue( UartMsgBuffer *buffer, char msg )
{
	if( buffer->count == (UART_MSG_BUFF_SIZE-1) )
		return FULL;

	buffer->count++;
	buffer->queue[ buffer->start_pointer++ ] = msg;

	if( buffer->start_pointer >= UART_MSG_BUFF_SIZE )
		buffer->start_pointer -= UART_MSG_BUFF_SIZE;

	return FILLED;
}

static UartBuffStat DeQueue( UartMsgBuffer *buffer, char *data )
{
	if( buffer->count == 0 )
		return EMPTY;

	buffer->count--;
	*data = buffer->queue[ buffer->end_pointer++ ];

	if( buffer->end_pointer >= UART_MSG_BUFF_SIZE )
		buffer->end_pointer -= UART_MSG_BUFF_SIZE;

	return FILLED;
}


