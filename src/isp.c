#include "isp.h"
#include "config.h"
#include "delay.h"

unsigned char isp_write(unsigned char ibuf)
{
    unsigned char res;
    unsigned char i = 8;
    res = 0;
    while(i--)
    {        
        if(ibuf & 0x80)
            ISP_MOSI = 1;
        else ISP_MOSI = 0;
        ibuf<<=1;
        ISP_SCLK = 1;
        res<<=1;
        res |= ISP_MISO;                
        ISP_SCLK = 0;
    }
    return res;
}


//void isp(char a, char b, char c, char d)
//{
//    obuf[0] = isp_write(a);
//    obuf[1] = isp_write(b);
//    obuf[2] = isp_write(c);
//    obuf[3] = isp_write(d);
//}
void isp_stop()
{
   ISP_RESET_TRIS = 1;
   ISP_MOSI_TRIS =1;
   ISP_MISO_TRIS =1;
}
unsigned char isp_start(void)
{
    unsigned char n=4;

    ISP_RESET_TRIS=1;
            ISP_MOSI_TRIS =0;
            ISP_MISO_TRIS =1;
    if(ISP_RESET_IN) //anything is  connected
    {        
        while(n--)
        {
            ISP_RESET_TRIS = 0;
            ISP_RESET = 0;
            __delayMs(15);
            isp(0xac,0x53,0,0);
            __delayMs(15);
            if (obuf[2] == 0x53)
                return 1;
            ISP_RESET_TRIS = 1;
        }
    }
    return 0;
}

void isp_close()
{
    ISP_RESET_TRIS = 1;
}
unsigned char ispReadFlashByte(unsigned int address) {
    
    isp_write((address & 1) ? 0x28:0x20);
    isp_write((unsigned char)(address >> 9));
    isp_write((unsigned char)(address >> 1));
    return isp_write(0);
}
unsigned char ispWriteFlashByte(unsigned int address, unsigned char data) {

    isp_write((address & 1) ? 0x48:0x40);
    isp_write((unsigned char)(address >> 9));
    isp_write((unsigned char)(address >> 1));
    isp_write(data);
    return 0;
}

unsigned char ispReadEEPROMByte(unsigned int address) {

    isp_write(0xA0 );
    isp_write((unsigned char)(address >> 8));
    isp_write((unsigned char)(address ));
    return isp_write(0);
}

unsigned char ispWriteEEPROMByte(unsigned int address, unsigned char data) {
    isp_write(0xC0 );
    isp_write((unsigned char)(address >> 8));
    isp_write((unsigned char)(address ));
    isp_write(data);
    return 0;
}
unsigned char ispFlushPage(unsigned int address) {
    isp_write(0x4C);
    isp_write((unsigned char)(address >> 9));
    isp_write((unsigned char)(address >> 1));
    isp_write(0);
    __delayMs(5);
    return 0;
}
void ispEraseChip()
{
    isp(0xac,0x80,0,0);
    __delayMs(10);
    unsigned char i=5;
    while(i--) {
        ISP_RESET_TRIS = 1;
        __delayMs(5);
        ISP_RESET_TRIS = 0;
        ISP_RESET = 0;
        __delayMs(5);
    }
}