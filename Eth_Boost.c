
#include "Eth_Boost.h"
inline void SPI_Write(unsigned int addr, unsigned char data)
{
  char Tmp;
  // Activate the CS pin
  SPI_PORT &= ~(1 << SPI_CS);
  // Start Wiznet W5100 Write OpCode transmission
  SPDR = WIZNET_WRITE_OPCODE;
  Tmp = (addr & 0xFF00) >> 8;
  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF)));
  // Start Wiznet W5100 Address High Bytes transmission
  SPDR = Tmp;
  Tmp = addr & 0x00FF;
  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF)));
  // Start Wiznet W5100 Address Low Bytes transmission
  SPDR = Tmp;
  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF)));

  // Start Data transmission
  SPDR = data;
  Tmp = SPI_PORT | (1 << SPI_CS);
  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF)));
  // CS pin is not active
  SPI_PORT = Tmp;
}
inline unsigned char SPI_Read(unsigned int addr)
{
  char Tmp;
  // Activate the CS pin
  SPI_PORT &= ~(1 << SPI_CS);
  // Start Wiznet W5100 Read OpCode transmission
  SPDR = WIZNET_READ_OPCODE;
  // Wait for transmission complete

  while (!(SPSR & (1 << SPIF)))Tmp = (addr & 0xFF00) >> 8;
  // Start Wiznet W5100 Address High Bytes transmission
  SPDR = Tmp;

  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF)))Tmp = addr & 0x00FF;
  // Start Wiznet W5100 Address Low Bytes transmission
  SPDR = Tmp;
  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF)));

  // Send Dummy transmission for reading the data
  SPDR = 0x00;

  // Wait for transmission complete
  while (!(SPSR & (1 << SPIF))) Tmp = SPI_PORT | (1 << SPI_CS);

  // CS pin is not active
  SPI_PORT = Tmp;
  return (SPDR);
}
unsigned int SPI_Read16(unsigned int addr)
{
  unsigned int L = 0;
  L = SPI_Read(addr) << 8;
  L |= SPI_Read(addr + 1);
  return L;
}
unsigned int SPI_Write16(unsigned int addr, unsigned int data)
{
  SPI_Write(addr  , data >> 8);
  SPI_Write(addr + 1, data);
}

unsigned int RECVData(RingBuff* rb,byte _sock)
{
  unsigned int RL;
  if ((rb->tail) > (rb->head))
  {
    //|b| |h| | | | |t|e|  (head,end-head+1,buffer,tail-buffer)=(head,2,buffer,2)
    rb->head = READALL(_sock,rb->head, rb->tail - rb->head - 1, 0, 0, &RL);
  }
  else
  { //|b| |t| | | | |h|e|  (head,end-head+1,buffer,tail-buffer)=(head,2,buffer,2)
    if (rb->tail == rb->buffer)
    {
      rb->head = READALL(_sock,rb->head, rb->buffer_end - rb->head, 0, 0, &RL);

    }
    else
    {
      rb->head = READALL(_sock,rb->head, rb->buffer_end - rb->head + 1, rb->buffer, rb->tail - rb->buffer - 1, &RL);
    }
    if (rb->head == rb->buffer_end + 1)rb->head = rb->buffer;

    //RL=123;
  }
  return RL;


}
inline unsigned int SPI_ReadF(unsigned int addr, unsigned int addrbase, unsigned int addrtop, char* WBuff, char* WEndP1)
{
  for (; WBuff != WEndP1;)
  {
    char Tmp;
    // Activate the CS pin
    SPI_PORT &= ~(1 << SPI_CS);
    // Start Wiznet W5100 Read OpCode transmission
    SPDR = WIZNET_READ_OPCODE;
    // Wait for transmission complete

    while (!(SPSR & (1 << SPIF)))Tmp = (addr & 0xFF00) >> 8;
    // Start Wiznet W5100 Address High Bytes transmission
    SPDR = Tmp;

    // Wait for transmission complete
    while (!(SPSR & (1 << SPIF)))Tmp = addr & 0x00FF;
    // Start Wiznet W5100 Address Low Bytes transmission
    SPDR = Tmp;
    // Wait for transmission complete
    while (!(SPSR & (1 << SPIF)));

    // Send Dummy transmission for reading the data
    SPDR = 0x00;

    // Wait for transmission complete
    while (!(SPSR & (1 << SPIF))) Tmp = SPI_PORT | (1 << SPI_CS);

    // CS pin is not active
    SPI_PORT = Tmp;
    *WBuff = SPDR;
    WBuff++;
    if (addrtop == addr)addr = addrbase;
    else             addr++;
  }
  return (addr);
}
void ssdsd()
{
 /* gS0_RX_BASE = chip_base_address + RX_memory_base_address(0x6000); gS0_RX_MASK = 2K – 1 ; // 0x07FF, 在指定的Socket 0接收内存取得偏移地址
  gS1_RX_BASE = gS0_BASE + (gS0_MASK + 1); gS1_RX_MASK = 2K – 1 ; 
  gS2_RX_BASE = gS1_BASE + (gS1_MASK + 1); gS2_RX_MASK = 2K – 1 ; 
  gS3_RX_BASE = gS2_BASE + (gS2_MASK + 1); gS3_RX_MASK = 2K – 1 ;*/
}

/*unsigned int CC_ADDR;
unsigned int CC_L;
unsigned int CC_PADDR;
unsigned int CC_PADDR2;*/
char* READALL(byte _sock,char *buff1, unsigned int MaxReadL1, char *buff2, unsigned int MaxReadL2, unsigned int *ReadL)
{ // return Last pointer

  unsigned int gS0_RX_MASK;
  unsigned int Base;
  unsigned int AddrTop;
  unsigned int L;
  unsigned int Addr;
  //unsigned int AddrC;
  char *LastPtr;
  char * DestPtr;

  noInterrupts(); 

  *ReadL = 0;
  gS0_RX_MASK = 0x7FF; //2K-1
  Base = 0x6000+_sock*2048;
  AddrTop = Base + gS0_RX_MASK;
  
#define Sn_RX_RSR(n) (0x0426+0x0100*n)
#define Sn_RX_RD(n) (0x0428+0x0100*n)
#define Sn_CR(n) (0x0401+0x0100*n)
#define Sn_CR_RECV 0x40
  
  
  L = SPI_Read16(Sn_RX_RSR(_sock));
  //while(L>4000)L = SPI_Read16(S0_RX_RSR);
  if (L == 0)
  {
    
    interrupts(); 
    return buff1;
  }

  Addr = SPI_Read16(Sn_RX_RD(_sock));
 // CC_ADDR = Addr;
 // CC_L = L;
  
  if (L < MaxReadL1)
  {
    MaxReadL1 = L;
    MaxReadL2 = 0;
  }
  else if (L < MaxReadL1 + MaxReadL2)
    MaxReadL2 = L - MaxReadL1;
  else
    L = MaxReadL1 + MaxReadL2;

  SPI_Write16( Sn_RX_RD(_sock) , Addr + L);
  *ReadL=L;//L;
  Addr = (Addr & gS0_RX_MASK);
 // CC_PADDR = Addr;
  Addr += Base;

  LastPtr = buff1; DestPtr = LastPtr + MaxReadL1;
  Addr = SPI_ReadF(Addr, Base, AddrTop, LastPtr, DestPtr);
  /*for(;LastPtr!=DestPtr;)
  {
    *(LastPtr)=SPI_Read(Addr);
    LastPtr++;
    if(AddrTop==Addr)Addr=Base;
    else             Addr++;
  }*/
  if (MaxReadL2)
  {
    LastPtr = buff2;
    DestPtr = LastPtr + MaxReadL2;
    Addr = SPI_ReadF(Addr, Base, AddrTop, LastPtr, DestPtr);
    /*
    for(;LastPtr!=DestPtr;)
    {
      *(LastPtr)=SPI_Read(Addr);
      LastPtr++;
      if(AddrTop==Addr)Addr=Base;
      else             Addr++;
    }*/
  }
  LastPtr = DestPtr;

  //AddrC
  SPI_Write( Sn_CR(_sock) , Sn_CR_RECV);
  interrupts(); 
  return LastPtr;
}
unsigned int FindPkgHead(char *buff, unsigned int idx_Start, unsigned int BoundMask, unsigned int idx_Stop, char *IP_2)
{
  unsigned int next_iS;

  for (; idx_Start != idx_Stop; idx_Start = next_iS)
  {
    next_iS = (idx_Start + 1)&BoundMask;
    if (buff[idx_Start] == IP_2[0] && buff[next_iS] == IP_2[1]) return idx_Start;
  }

  return idx_Stop;
}
unsigned int FindPkgEnd(char *buff, unsigned int idx_H, unsigned int BoundMask, unsigned int idx_Stop, char *IP_2)
{ //192 168 1 2 23 112 0 2 5 11

  unsigned int idx_E = (idx_H + 6)&BoundMask;

  unsigned int PkgL = buff[idx_E] << 8;
  idx_E = (idx_E + 1)&BoundMask;
  PkgL |= buff[idx_E];
  idx_E = (idx_H + PkgL + 2)&BoundMask;

  PkgL = FindPkgHead(buff, idx_E, BoundMask, idx_Stop, IP_2);
  if (PkgL == idx_E)return idx_E;
  return idx_H;
}

/*
void READALLk()
{

  unsigned int gS0_RX_MASK = 2*1024-1;
  unsigned int Base =0x6000;
  unsigned int AddrTop = Base+gS0_RX_MASK;

  unsigned int L=SPI_Read16(S0_RX_RSR);
  if(!L)return;
  unsigned int Addr=SPI_Read16(S0_RX_RD);


    SPI_Write16( S0_RX_RD ,Addr+L);
   Addr+= Base;
  for(;L;L--)
  {
    char D=SPI_Read(Addr);
      if(Addr==AddrTop)Addr=Base;
      else Addr++;
  }
   SPI_Write( 0x401 ,0x40);

}*/
