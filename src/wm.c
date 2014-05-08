/* 
 * File:   wm.c
 * Author: dmitry
 *
 * 
 * Created on 8. leden 2012, 1:14
 */

#include "pic18.h"
#include "config.h"
#include "htc.h"
#include <stdio.h>
#include <stdlib.h>

#include    "wm.h"
#include "isp.h"
#include "flash.h"
#include "crc.h"
#include "uart.h"
#include "delay.h"


/*CONFIG*/
__CONFIG(1, RCIO
        &FCMEN);

__CONFIG(2, BORV22
        &SWBOREN
        &PWRTEN
        &WDTEN);

__CONFIG(3, MCLRDIS
        &HFSTART
        &LPT1DIS
        &PBDIGITAL
        &CCP2RB3);

__CONFIG(4, DEBUGDIS
        &XINSTDIS
        &LVPDIS
        &STVREN);
__CONFIG(5, UNPROTECT);
/*END CONFIG*/



unsigned char block_buf[1024];
unsigned char block_buf2[1024];

unsigned char atmel_blocks_count = 32;

static t_sTable data_table;

/*
 * 
 */
void main(void) {


    init();
    __delayMs(100);

    while (!isp_start()) {
        LED_GREEN = !LED_GREEN;
        CLRWDT();
    }
    LED_GREEN=0;


       if (first_start() || !JP1) //first start
        {
            data_table.synchronized_chip_id=0;
            data_table.block_count = 0;
            data_table.crc = 0;

            read_flash_and_create_table();
            save_data_table(); //store to EEPROM
        }
        else
        {
               if(restore_data_table())
               {
                compare_atmel_with_stored_data();
               }
        }
    
    isp_stop();
    SLEEP();
    while (1) CLRWDT();

}

static void init(void) {
    
#ifdef XTAL_INTERNAL
    setFrequency(_16MHz);
#endif

    TRISA = 0;

    ANSEL = 0;
    ANSELH = 0;
    //setup ISP
    TRISCbits.TRISC4 = 0; //MOSI
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB3 = 0; 
    TRISCbits.TRISC3 = 1; //MISO
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB1 = 1;

    //interrupts

    IOCB = 0xff;
    PIR1 = 0;
    PIR2 = 0;
    INTCON = 0b00100000;
    INTCON2bits.RBPU = 0;
    T0CON = 0b10010010;

    /*UART*/
    TXSTA = 0b00100110;
    RCSTA = 0b10010000;
    BAUDCTL = 0b00001000;
    SPBRG = table_speed[BAUD115200]; //19200

    GIEH = 1;
    GIEL = 1;

}

static void interrupt isr_high(void) {/*Interrupts*/

    /*
     * Timer 0
     */
    /*1 ms timer*/
    if (TMR0IF && TMR0IE) //timer for signal
    {
        TMR0 = -500;
        TMR0IF = 0;
        DELAY_MACRO;
    }


}

void save_data_table() {
    unsigned char iee;
    unsigned char sum=0;
    unsigned char *p=(unsigned char*) &data_table;
    for (iee = 0; iee<sizeof (t_sTable);iee++) {
        sum+=*p;
       eeprom_write(iee, *p);
       p++;
    }
    eeprom_write(sizeof(t_sTable),sum);
}

unsigned char restore_data_table() {
    unsigned char iee;
    unsigned char sum=0;
    unsigned char *p = (unsigned char*) &data_table;
    for (iee = 0; iee<sizeof (t_sTable);iee++) {
        *p = eeprom_read(iee);
        sum+=*p;
        p++;
    }
    if(sum == eeprom_read(sizeof (t_sTable)))
    {
        return 1;
    }
    return 0;
}

unsigned char first_start() {
    if ( eeprom_read(0)==0xff) {
        return 1;
    }
    return 0;
}

int read_atmel_data_block1024(unsigned int adr_inatmel, unsigned char type) {
    unsigned int indx;
    unsigned char empty = 1, sum = 0;

    LED_GREEN=1;
    if (type == eDataFlash) {
        for (indx = 0; indx < 1024;) {
            block_buf[indx] = ispReadFlashByte(adr_inatmel++);
            #ifndef CRC-8
            sum += block_buf[indx];
            #endif
            if (block_buf[indx] != 0xff)
                empty = 0;
            indx++;
        }
        #ifdef CRC-8
            sum = calculate_sum_1024bytes(block_buf);
        #endif
    } else if (type == eDataEEPROM) {
        for (indx = 0; indx < 1024;) {
            block_buf[indx] = ispReadEEPROMByte(indx);
        #ifndef CRC-8
            sum += block_buf[indx];
        #endif
            if (block_buf[indx] != 0xff)
                empty = 0;
            indx++;
        }
        #ifdef CRC-8
            sum = calculate_sum_1024bytes(block_buf);
        #endif
    }
    LED_GREEN=0;
    if (empty)
        return EMPTY_BLOCK;

    return (unsigned int) sum;

}

int write_atmel_data_block1024(unsigned int adr_inmicrochip, unsigned int adr_inatmel, unsigned char type) {

    unsigned char isb; //index subblock
    unsigned char isbb; //index subblock byte
    unsigned char empty = 1;
    unsigned char sum = 0; //cur_crc
    unsigned int block_adr;
    LED_RED = 1;

    if (type == eDataFlash) {
        isb = BLOCK_SIZE / 64;
        while (isb--) {

            readFlash64b(adr_inmicrochip, (unsigned char*)block_buf); //read 64 bytes form
            adr_inmicrochip += 64;
            block_adr= adr_inatmel;
            for (isbb = 0; isbb < 64; isbb++) {
                ispWriteFlashByte(adr_inatmel, block_buf[isbb]);
                adr_inatmel++;
            }
           ispFlushPage(block_adr);
        }

    } else if (type == eDataEEPROM) {

        isb = BLOCK_SIZE / 64;
        while (isb--) {
            readFlash64b(adr_inmicrochip, block_buf); //read 64 bytes form            

            for (isbb = 0; isbb < 64; isbb++) {
                do{
  
                 ispWriteEEPROMByte(adr_inatmel, block_buf[isbb]);
                    __delayMs(5);
                }while(ispReadEEPROMByte(adr_inatmel)!=block_buf[isbb]);
                adr_inatmel++;
                adr_inmicrochip += 64;
            }
           
        }
    }
    LED_RED = 0;
    return (unsigned int) sum;

}

unsigned char save_to_flash(unsigned char bank_number) {
    unsigned int adr = getBankAddress(bank_number);
    unsigned char *ptr = block_buf;
    unsigned char res=0;

    for (unsigned char ii = 0; ii < BLOCK_SIZE/64; ii++) {
        res |= writeFlash64b(ptr, adr);
        ptr += 64;
        adr += 64;
    }
   
    return res;
}

void read_flash_and_create_table() {
    unsigned int cur_adr, cur_crc;
    /*flash
     */ 
     unsigned char   ib = 0;
    cur_adr=0;
    do {

        cur_crc = read_atmel_data_block1024(cur_adr, eDataFlash);

        if (cur_crc != EMPTY_BLOCK) {
            data_table.block[data_table.block_count].adr = cur_adr;
            data_table.block[data_table.block_count].opt.crc = (unsigned char) cur_crc;
            data_table.block[data_table.block_count].opt.allocation = eDataFlash;

            save_to_flash(data_table.block_count);

            if (data_table.block_count++ >= MAX_BLOCKS_FOR_SAVE); //return ERROR;
        }
        cur_adr += 1024;
        ib++;
    } while (ib < atmel_blocks_count);
    
    __delayMs(100);
    /*EEPROM
     */
    cur_crc = read_atmel_data_block1024(0, eDataEEPROM);

    if (cur_crc != EMPTY_BLOCK) {
        data_table.block[data_table.block_count].opt.crc = (unsigned char) cur_crc;
        data_table.block[data_table.block_count].opt.allocation = eDataEEPROM;
        save_to_flash(data_table.block_count);
        data_table.block_count++;
    }


        


}

int compare_atmel_with_stored_data() {
    unsigned int cur_crc;
   unsigned char   try_check; //check tries
           unsigned char ib = 0; //table index
   unsigned char need_to_repair=0; 
 
    /*flash
     */
   try_check=2;
   do{

    while (ib < data_table.block_count) {

        cur_crc = read_atmel_data_block1024(data_table.block[ib].adr
                , data_table.block[ib].opt.allocation);


        if (data_table.block[ib].opt.crc != (unsigned char) cur_crc
                && data_table.block[ib].opt.allocation!=eDataEEPROM) {
                 need_to_repair=1;
                 break;
        }
        ib++;
    }

        if(need_to_repair)
        {
            start_write_atmel();
            ib=0;
            while(ib<data_table.block_count)
            {
                write_atmel_data_block1024(getBankAddress(ib)
                        , data_table.block[ib].adr
                        , data_table.block[ib].opt.allocation);
                ib++;
            }
            need_to_repair=0;
        }
        else 
            return 1;


   }while(try_check-- );

        return 0;

}

void start_write_atmel()
{
    ispEraseChip();
    while(! isp_start()) CLRWDT();
}