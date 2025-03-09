#include "ringBuffer.h"

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

void ringBuffer::enqueueBlock(uint8_t *data, uint16_t nBytes){
  uint16_t i;

  for (i=0;i<nBytes;i++) enqueue(data[i]);
}

uint16_t ringBuffer::dequeueBlock(uint8_t *data, uint16_t maxSize){
  bool ok;
  uint8_t d;
  uint16_t i;
  
  for (i=0;i<maxSize;i++){
    d=dequeue(&ok);
    if (ok){
      data[i]=d;
    }else{
      return i;
    }
  }

  return maxSize;
}