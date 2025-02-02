#include "usbEndpoint.h"
#include <string.h>
#include "usbDev.h"

usbEndpoint::usbEndpoint(uint16_t bufferSz, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType){
  bufferSize=bufferSz;
  size=sz;
  dir=uDir;
  type=uType;
  
  usbHardware=nullptr;
  inBuffer=nullptr;
  outBuffer=nullptr;
  m_endpointNumber=0x0F;
}

void usbEndpoint::setHardware(usbDev *hw){
	usbHardware=hw;
}

void usbEndpoint::initialise(){
  if (usbHardware==nullptr) return;

  if (dir==usbEndpoint::endpointDirection::in  || type==usbEndpoint::endpointType::control || type==usbEndpoint::endpointType::dual) usbHardware->allocateEndpointBuffer(&inBuffer, bufferSize);
  if (dir==usbEndpoint::endpointDirection::out || type==usbEndpoint::endpointType::control || type==usbEndpoint::endpointType::dual) usbHardware->allocateEndpointBuffer(&outBuffer, bufferSize);
  
  descriptor.bLength          = 7;                                     // Size of descriptor (always 9)
  descriptor.bDescriptorType  = (uint8_t)usbDescriptorTypes::endpoint; // Interface descriptor Type  - USB Standard page 251

  descriptor.bEndpointAddress=m_endpointNumber & 0x0F;
  
  switch (dir){
    case usbEndpoint::endpointDirection::in:
      descriptor.bEndpointAddress += 0x80;
      break;
    case usbEndpoint::endpointDirection::out:
      descriptor.bEndpointAddress += 0x00;
      break;
  };

  descriptor.bmAttributes=0;
  switch (type){
    case usbEndpoint::endpointType::control:
      descriptor.bmAttributes += 0x00;
      break;
    case usbEndpoint::endpointType::isochronos:
      descriptor.bmAttributes += 0x01;
      break;
    case usbEndpoint::endpointType::bulk:
      descriptor.bmAttributes += 0x02;
      break;
    case usbEndpoint::endpointType::interrupt:
      descriptor.bmAttributes += 0x03;
      break;
  };

  /// ToDo: Handle Isochronos configuration of bmAttributes bits 2 to 5.

  descriptor.wMaxPacketSize   = (uint16_t)size & 0x07FF;        // Maximum packet size
  descriptor.bInterval        = 10;                             // Interval between packets
}

void usbEndpoint::setHardwareEndpoint(uint8_t hardwareEndpoint){
  m_endpointNumber=hardwareEndpoint;
  descriptor.bEndpointAddress=m_endpointNumber & 0x0F; // Update descriptor at same time
}

uint8_t usbEndpoint::hardwareEndpoint(){
  return m_endpointNumber;
}

void usbEndpoint::setupRecieved(uint16_t nBytes){
}

void usbEndpoint::dataRecieved(uint16_t nBytes){		
}
	
uint16_t usbEndpoint::writeIn(uint8_t *data, uint16_t nBytes, uint16_t maxLength){
  uint16_t msgLen=nBytes;
  if (usbHardware==nullptr) return 0;

  if (msgLen>maxLength) msgLen=maxLength;
  if (msgLen>bufferSize) msgLen=bufferSize;
  
  memcpy(inBuffer, data, msgLen);

  return usbHardware->writeIn(descriptor.bEndpointAddress, msgLen);
}

void usbEndpoint::writeInZLP(){
  if (usbHardware==nullptr) return;
  usbHardware->writeInZLP(descriptor.bEndpointAddress);
}

void usbEndpoint::writeInStall(){
  if (usbHardware==nullptr) return;
  usbHardware->writeInStall(descriptor.bEndpointAddress);
}