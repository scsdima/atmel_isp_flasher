#include "uart.h"


/* sends a string n-number bytes*/
void  sci_PutStr(const char *str,unsigned int n)
{
    while(n--)
            {
            while(!TXIF) ;
            TXREG = *(str++);
            }
}

void sci_PutChar( char c)
{
	while(!TXIF) ;
	TXREG = c;	
}

int sci_GetChar(void )
{
    unsigned char ch = RCREG;
    if( byteReceived() )
            RCIF = 0;
    return -1;

    if ( OERR ) /*error occurs*/
    {
            fetchUART();           
            return -1;
    }
    return ch;
}

