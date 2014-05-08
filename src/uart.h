#ifndef ___UART_H
    #define ___UART_H
    #include <pic18.h>



    enum{
        BAUD19200  =0,
        BAUD57600  = 1,
        BAUD115200  =2
    };
    
    //#ifdef XTAL_INTERNAL

       const int table_speed[] ={207 //19200
                                   ,68//57600
                                   ,34};//115200
   // #else

   // #endif

    void sci_PutStr(const char *,unsigned int );

    int  sci_GetChar(void );

    void sci_PutChar( char);

    #define byteReceived()	RCIF

    #define fetchUART() do{	CREN = 0;	\
                          RCREG;		\
                            RCREG;		\
                            CREN = 1;     }while(0);
#endif 