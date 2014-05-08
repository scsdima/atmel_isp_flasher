#ifndef WM_H
    #define WM_H

    #include "htc.h"
    #include "pic18.h"


    #define ERROR           -1
    #define EMPTY_BLOCK     -2

    #define		_16MHz		(0b01110000)
    #define		_8MHz		(0b01100000)
    #define		_4Mhz		(0b01010000)
    #define		_2MHz		(0b01000000)
    #define		_1MHz		(0b00110000)
    #define		_500kHz		(0b00100000)
    #define		_250kHz		(0b00010000)
    #define		_31kHz		(0b00000000)

    #define		setFrequency(X)	do{OSCCON |= (X);}while(0)


    //typedef t_sOpt;
        enum {eDataFlash=0x01,eDataEEPROM=0x02};
    typedef struct 
    {
      struct {
            unsigned char allocation;
            unsigned char crc;
        }opt;
        unsigned int adr;
    }t_sDataBlock;

    #define     BLOCK_SIZE  1024
    #define     MAX_BLOCKS_FOR_SAVE 60
    #define     BLOCKS_OFFSET 0x1000
    typedef struct
    {

        unsigned char synchronized_chip_id;
        unsigned int crc;
        unsigned char block_count;
        t_sDataBlock block [MAX_BLOCKS_FOR_SAVE];
    }t_sTable;


    static void  init(void);

    void save_data_table();

    unsigned char restore_data_table();

    unsigned char first_start();

    int  read_atmel_data_block1024( unsigned int,unsigned char);

    int  write_atmel_data_block1024( unsigned int,unsigned int,unsigned char);
    
    unsigned char save_to_flash(unsigned char);

    void read_flash_and_create_table();

    int compare_atmel_with_stored_data();
    
    void stop_write_atmel();

    void start_write_atmel();

    #define getBankAddress(_bank_) (BLOCKS_OFFSET+(_bank_)*1024)


    
#endif