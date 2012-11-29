#include "uart.h"
#include <msp430.h>

static unsigned int g_timeout = UART_DEFAULT_TIMEOUT;

void UartInit(unsigned char Baud)
{
    // UART initialization
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;
    P1SEL |= (BIT1|BIT2);             // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= (BIT1|BIT2);
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = Baud;                           // 1MHz
    UCA0BR1 = 0;                              // 1MHz
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    //?IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}


static int UartSendByte(unsigned char data, unsigned char wait)
{
    UCA0TXBUF = data;
    if (wait)
    {
        unsigned int t = g_timeout;
        do
        {
            if (0 == --t)
                return -1; // timed out
        } while (!(IFG2&UCA0TXIFG));
    }
    return 1; // OK
}


static int UartRecvByte(unsigned char * data, unsigned char wait)
{
    (*data) = 0;
    if (wait)
    {
        unsigned int t = g_timeout;
        do
        {
            if (0 == --t)
                return -1; // timed out
        } while (!(IFG2&UCA0RXIFG));
    }
    else
    {
        if (!(IFG2&UCA0RXIFG))
            return 0; // no data yet
    }

    (*data) = UCA0RXBUF;
    return 1; // OK
}


int UartRecvBytes(unsigned char * data, int length)
{
    int i, r;
    for (i = 0; i < length; ++i)
    {
        r = UartRecvByte(&data[i], 1); // (!) wait for data
        if (r != 1)
            return -1;
    }
    return length;
}


int UartSendBytes(const unsigned char * data, int length)
{
    int i, r;
    for (i = 0; i < length; ++i)
    {
        r = UartSendByte(data[i], 1); // (!) wait for data
        if (r != 1)
            return -1;
    }
    return length;
}


void SetReadTimeout(unsigned int timeout)
{
    g_timeout = timeout;
}
