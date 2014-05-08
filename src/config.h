#ifndef CONFIG_H
	#define CONFIG_H



        #include 	"htc.h"
        #include "pic18.h"


	


    #define     ISP_INTERFACE
    #define     ISP_RESET   LATBbits.LATB0
    #define     ISP_RESET_IN   PORTBbits.RB0
    #define     ISP_RESET_TRIS TRISBbits.TRISB0
    #define     ISP_MOSI    LATCbits.LATC4
    #define     ISP_MISO    PORTCbits.RC3
    #define     ISP_MOSI_TRIS    TRISCbits.TRISC4
    #define     ISP_MISO_TRIS    TRISCbits.TRISC3
    #define     ISP_SCLK    LATBbits.LATB3


    //common
    #define LED_RED    LATAbits.LATA2
    #define LED_GREEN    LATAbits.LATA0
    #define JP1         PORTBbits.RB2
    #define JP2         PORTBbits.RB1


    #define     XTAL_INTERNAL

#endif