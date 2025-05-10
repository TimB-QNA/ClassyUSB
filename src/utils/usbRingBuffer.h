#ifndef USBRINGBUFFER_H_
#define USBRINGBUFFER_H_

#include <stdint.h>

class usbRingBuffer  
{
  public:
    usbRingBuffer();
    usbRingBuffer(uint8_t *buffer, uint16_t buffSize);
    
    void clear();
    
    bool init(uint8_t *buffer, uint16_t buffSize);
    
    bool isEmpty();
    bool isFull();
    
    bool    enqueue(uint8_t data);
    uint8_t dequeue(bool *ok=nullptr);

    uint16_t enqueueBlock(uint8_t *data, uint16_t nBytes);
    uint16_t dequeueBlock(uint8_t *data, uint16_t maxSize);
    
  private:
    uint8_t *m_buffer;
    uint16_t m_bufferSize;
    uint16_t m_head;
    uint16_t m_tail;
};

#endif
