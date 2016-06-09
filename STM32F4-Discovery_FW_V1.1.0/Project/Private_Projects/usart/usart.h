#include "stm32f4xx.h"

//int EnQueue( char *buff, int *sptr, int *eptr, char msg );
//int DeQueue( char *buff, int *sptr, int *eptr, char *data );

void uart4_hw_init( void );
void SendMessage( char *msg );
void ReceiveMessage( char *msg );	// not implemented yet.
void UART4_IRQHandler( void );

