#ifndef __USART_16F88_H__
#define __USART_16F88_H__

#define USART_BAUD_9600     51 //25
#define USART_BAUD_19200    25 //12
#define USART_BAUD_28800    16 //6
#define USART_BAUD_38400    12 //3
#define USART_BAUD_57600    8 //1

void UsartInit(unsigned char Baud);
unsigned char UsartSendByte(unsigned char data, unsigned char wait);
unsigned char UsartRecvByte(unsigned char * data, unsigned char wait);
//unsigned char UsartRecvBytes(unsigned char * data, short length, unsigned char wait);
int UsartRecvBytes(unsigned char * data, int length);

unsigned char UsartSendString(unsigned char * s);
//unsigned char UsartSendBytes(unsigned char * data, short length);
int UsartSendBytes(const unsigned char * data, int length);

#define DEFAULT_TIMEOUT  0x1000

void SetReadTimeout(unsigned int timeout);

//unsigned char ReceiveHex(unsigned char * hexdata, unsigned char wait);
//void SendHex(unsigned char hexdata, unsigned char wait);

#endif
