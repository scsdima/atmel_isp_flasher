#ifndef ___FLASH_H
    #define ___FLASH_H
    
    unsigned char writeFlash64b(const unsigned char* data,unsigned int dst);

    void readFlash64b(unsigned int src_flash,unsigned char *);

#endif