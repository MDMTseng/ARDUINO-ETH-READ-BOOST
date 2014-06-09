#include "RingBuff.h"

void cb_init(RingBuff *rB, char *buff,unsigned int Length)
{
    rB->head =
    rB->tail =
    rB->buffer = buff;
    rB->capacity =Length;
    rB->buffer_end=buff+Length-1;
    rB->count=0;
}

char cb_push_back(RingBuff *rB, char data,char IsForce)
{
  char* nextAddr;
  if(rB->head==rB->buffer_end)nextAddr=rB->buffer;
  else nextAddr=rB->head+1;
  if(nextAddr==rB->tail)//Buff Full decide abort or forcing push
  {
    if(!IsForce)return 0;
    if(nextAddr==rB->buffer_end)rB->tail=rB->buffer;
    else rB->tail=nextAddr+1;
  }
  *(rB->head)=data;
  rB->head=nextAddr;
  return 1;
  
}

char* cb_pop_front(RingBuff *rB)
{
    char *retVar;
    if(rB->head==rB->tail)return 0;
    retVar=rB->tail;
    if(rB->tail==rB->buffer_end)rB->tail=rB->buffer;
    else rB->tail=rB->tail+1;
    
    return retVar;
    
}
