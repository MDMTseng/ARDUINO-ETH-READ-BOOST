#ifndef ETH_BOOST_H
#define ETH_BOOST_H

#ifdef __cplusplus
 extern "C" {
#endif
//#include <Ethernet.h>
#include <Arduino.h>
#include "RingBuff.h"    
//#include <w5100.h>
#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SPI_CS   PORTB2
// Wiznet W5100 Op Code
#define WIZNET_WRITE_OPCODE 0xF0
#define WIZNET_READ_OPCODE 0x0F
// Wiznet W5100 Register Addresses
#define MR   0x0000   // Mode Register
#define GAR  0x0001   // Gateway Address: 0x0001 to 0x0004
#define SUBR 0x0005   // Subnet mask Address: 0x0005 to 0x0008
#define SAR  0x0009   // Source Hardware Address (MAC): 0x0009 to 0x000E
#define SIPR 0x000F   // Source IP Address: 0x000F to 0x0012
#define RMSR 0x001A   // RX Memory Size Register
#define TMSR 0x001B   // TX Memory Size Register

/*
typedef struct RingBuff {
    char*buff;
    unsigned int RFront,WFront;
    unsigned int L,count;
    
} RingBuff;*/


unsigned int SPI_Read16(unsigned int addr);
unsigned int SPI_Write16(unsigned int addr,unsigned int data);
inline void SPI_Write(unsigned int addr,unsigned char data);
inline unsigned char SPI_Read(unsigned int addr);
//void READALL();
unsigned int RECVData(RingBuff* rb,byte _sock);
char* READALL(byte _sock,char *buff1, unsigned int MaxReadL1, char *buff2, unsigned int MaxReadL2, unsigned int *ReadL);


#ifdef __cplusplus
}
#endif
#endif
