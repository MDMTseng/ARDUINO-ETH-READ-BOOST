#ifndef RINGBUFF_H
#define RINGBUFF_H
#ifdef __cplusplus
 extern "C" {
#endif

typedef struct RingBuff//the length should be 2^n  32 64 128 256...
{
    char *buffer;     // data buffer
    char *buffer_end; // end of data buffer
    unsigned int capacity;  // maximum number of items in the buffer
    unsigned int count;     // number of items in the buffer
    char *head;       // pointer to head
    char *tail;       // pointer to tail
} RingBuff;

void cb_init(RingBuff *rB, char *buff,unsigned int Length);

char cb_push_back(RingBuff *rB, char data,char IsForce);

char* cb_pop_front(RingBuff *rB);


#ifdef __cplusplus
}
#endif
#endif
