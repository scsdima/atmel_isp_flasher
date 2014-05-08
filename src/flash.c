#include "pic18.h"
#include"flash.h"

//-------------------------------------------------------------------------
unsigned char writeFlash64b(const unsigned char* data,unsigned int dst)
{
	unsigned char 	dc;
	unsigned char	_intcon;

	{
		/*ERASE_BLOCK */
		TBLPTR = (far unsigned char*) dst;
		EEPGD 	=1;
		CFGS 	=0;
		WREN	=1;
		FREE 	=1;
		EECON2 = 0x55;
		EECON2 = 0xaa;
		WR		=1;
		while(WR);
		WREN	=	0;
	}

		/*PROGRAM_MEMORY*/
	for(unsigned char sec=0,dc=0;sec<64;sec+=32)
	{
		unsigned char c=32;
		TBLPTRL	= (TBLPTRL&0b11000000) + sec;
		while( c-- )
		{
			TABLAT = data[dc++];
			asm("TBLWT*+");
		}
	//	TBLPTRL	= (TBLPTRL&0b11000000) + sec;
	asm("TBLRD*-");
		EEPGD 	=1;
		CFGS	=0;
		WREN 	=1;
		EECON2 	= 0x55;
		EECON2 	= 0xaa;
		WR=1;
		while(WR);
		WREN=0;
	}

	/*VERIFY MEMORY*/
	TBLPTRL &= 0b11000000;
	for(dc=0; dc<64; dc++)
		{
		asm("TBLRD*+");
		if(data[dc] != TABLAT)
			return 0xff;
		}

return 0;
}

void readFlash64b(unsigned int src_flash,unsigned char *buf)
{
    unsigned char dc;
    TBLPTR = (far unsigned char*) src_flash;
    TBLPTRL &= 0b11000000;
	for(dc=0; dc<64; dc++)
		{
		asm("TBLRD*+");
		*buf++ = TABLAT;
		}
 }