#ifndef __UART_H__
#define __UART_H__

#define UART_BAUD_9600  104

void UartInit(unsigned char Baud);
int UartRecvBytes(unsigned char * data, int length);
int UartSendBytes(const unsigned char * data, int length);

#define UART_DEFAULT_TIMEOUT  0x1000
void SetReadTimeout(unsigned int timeout);

#endif
