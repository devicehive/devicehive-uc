#include "usart_16f88.h"
#include <htc.h>

unsigned int _timeout = DEFAULT_TIMEOUT;

void UsartInit(unsigned char Baud)
{
    //OSCF = 0b1;
    OSCCON = 0b01111110;
    TRISB |= 0b00000110;
    SPBRG = Baud;
    TXSTA = 0b00000100;
    RCSTA = 0b10010000;
    SPEN = 0b1;
    SYNC = 0b0;

    //TXIE = 0b1;
    //RCIE = 0b1;

    TXEN = 0b1;
    CREN = 0b1;
}

unsigned char UsartSendByte(unsigned char data, unsigned char wait)
{
    //TXEN = 0b1;
    TXREG = data;
    if (wait)
    {
        unsigned int t = 0;
        do
        {
            ++ t;
            if (t >= _timeout) return 0;
        } while (TRMT == 0b0);
    }
    return 1;
}

unsigned char UsartRecvByte(unsigned char * data, unsigned char wait)
{
    //CREN = 0b1;
    //unsigned char rv = 0;
    (*data) = 0;
    if (wait)
    {
        unsigned int t = 0;
        do
        {
            ++t;
            if (t >= _timeout) return 0;
        } while (!RCIF);
    }
    else
    {
        if (!RCIF) return 0;
    }
    //rv = (OERR | (FERR << 1));
    if ((!OERR) && (!FERR))
    {
        (*data) = RCREG;
        return 1;
    }
    else
    {
        //if (OERR)
        {
            CREN = 0b0;
            CREN = 0b1;
        }
        return 0;
    }
    //CREN = 0b0;
    //return !rv;
}

int UsartRecvBytes(unsigned char * data, int length)
{
    int rv = 0;
    for (int x = 0; x < length; ++x)
    {
        unsigned char crv = UsartRecvByte(&data[x], 1);
        if (crv) rv ++;
        else break;
    }
    return rv;
}

/*
unsigned char ReceiveHex(unsigned char * hexdata, unsigned char wait)
{
    unsigned char ch;
    if (!UsartRecvByte(&ch, wait))
    {
        return 0;
    }
    *hexdata = (ch-0x30) * 16;
    if (!UsartRecvByte(&ch, wait))
    {
        return 0;
    }
    *hexdata += (ch-0x30);
    return 1;

}

void SendHex(unsigned char hexdata, unsigned char wait)
{
    UsartSendByte(hexdata / 16 + 0x30, wait);
    UsartSendByte(hexdata % 16 + 0x30, wait);
}
*/

unsigned char UsartSendString(unsigned char * s)
{
    unsigned char x = 0;
    while (s[x] != 0)
    {
        if (!UsartSendByte(s[x++], 1)) return 0;
    }
    return 1;
}

int UsartSendBytes(const unsigned char * data, int length)
{
    int rv = 0;
    for (int x = 0; x < length; ++x)
    {
        unsigned char crv = UsartSendByte(data[x], 1);
        if (crv) rv ++;
        else break;
    }
    return rv;
}

void SetReadTimeout(unsigned int timeout)
{
    _timeout = timeout;
}
