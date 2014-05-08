#ifndef ___ISP_H
    #define ___ISP_H

#include "config.h"

    #ifndef ISP_INTERFACE
        //#error "Setup ISP Interface"
    #endif




    

    unsigned char isp_start(void);

    void isp_close();

    unsigned char ispReadFlashByte(unsigned int address) ;

    unsigned char ispWriteFlashByte(unsigned int address, unsigned char data) ;

    unsigned char ispReadEEPROMByte(unsigned int address) ;

    unsigned char ispWriteEEPROMByte(unsigned int address, unsigned char data) ;


    unsigned char ispFlushPage(unsigned int address);

    void  ispEraseChip();

//private
    static unsigned char ibuf[4];
    static unsigned char obuf[4];

    void isp_stop();

    unsigned char isp_write(unsigned char ibuf);

   // void isp(char a, char b, char c, char d);
    #define isp(A,B,C,D) do{obuf[0] = isp_write(A);obuf[1] = isp_write(B);obuf[2] = isp_write(C);obuf[3] = isp_write(D);}while(0);

                        
#endif