#include "ringBuffer.h"
#include <string.h>

ringBuffer::ringBuffer(){
  m_buffer=nullptr;
  m_bufferSize=0;
  clear();
}

ringBuffer::ringBuffer(uint8_t *buffer, uint16_t buffSize){
  ringBuffer();
  init(buffer, buffSize);
}

void ringBuffer::clear(){
  m_head=0;
  m_tail=0;
}

bool ringBuffer::init(uint8_t *buffer, uint16_t buffSize){
  m_buffer = buffer;
  m_bufferSize=buffSize;
  clear();
  return true;
}  
  
bool ringBuffer::isEmpty(){
  return m_head == m_tail;
}

bool ringBuffer::isFull(){
  return ((m_head + 1) % m_bufferSize) == m_tail;
}  
  
bool ringBuffer::enqueue(uint8_t data){
  if (m_buffer==nullptr) return false;
  
  if (isFull()){
    // full.. scrap oldest.
    m_tail = (m_tail + 1) % m_bufferSize;
  }
  
  m_buffer[m_head] = data;
  m_head = (m_head + 1)  % m_bufferSize;
  
  return true;
}
    
uint8_t ringBuffer::dequeue(bool *ok){
  uint8_t data;
  
  if (ok!=nullptr) *ok=false;
  
  if (m_buffer==nullptr) return 0;
  if (isEmpty()) return 0;

  if (ok!=nullptr) *ok=true;
  
  data = m_buffer[m_tail];
  
  m_tail = (m_tail + 1) % m_bufferSize;
  return data;
}

uint16_t ringBuffer::enqueueBlock(uint8_t *data, uint16_t nBytes){
  uint16_t i;

  for (i=0;i<nBytes;i++) enqueue(data[i]);

  return nBytes;
}

uint16_t ringBuffer::dequeueBlock(uint8_t *data, uint16_t maxSize){
  bool ok;
  uint8_t d;
  uint16_t i, s;
  
  // Easiest condition, no data, return nothing...
  if (m_tail==m_head) return 0;

  // Easy condition, tail less than head so buffer is effectively a contiguous linear buffer.
  if (m_tail<m_head){
    i=m_head-m_tail;                  // Amount of data in buffer
    if (i>maxSize) i=maxSize;         // Limit to required size
    memcpy(data, m_buffer+m_tail, i); // Copy data
    m_tail+=i;                        // Update tail position.
    return i;
  }

  // At this point, tail is greater than head so we need to copy the top end of the array, then the bottom.
  i=m_bufferSize-m_tail;
  if (i>maxSize) i=maxSize;
  memcpy(data, m_buffer+m_tail, i);
  m_tail = (m_tail + i) % m_bufferSize;

  s=i;
  i=m_head;
  if (s+i>maxSize) i=maxSize-s;
  if (i==0) return i;
  memcpy(data+s, m_buffer, i);
  m_tail = i;
  
  return s+i;
}