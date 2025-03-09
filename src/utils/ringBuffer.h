/*
 * ringBuffer.h
 *
 * Created: 09/03/2025 18:36:15
 *  Author: Tim
 */ 

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>

class ringBuffer  
{
  public:
    ringBuffer();
    ringBuffer(uint8_t *buffer, uint16_t buffSize);
    
    void clear();
    
    bool init(uint8_t *buffer, uint16_t buffSize);
    
    bool isEmpty();
    bool isFull();
    
    bool    enqueue(uint8_t data);
    uint8_t dequeue(bool *ok=nullptr);

    void     enqueueBlock(uint8_t *data, uint16_t nBytes);
    uint16_t dequeueBlock(uint8_t *data, uint16_t maxSize);
    
  private:
    uint8_t *m_buffer;
    uint16_t m_bufferSize;
    uint16_t m_head;
    uint16_t m_tail;
};

#endif
