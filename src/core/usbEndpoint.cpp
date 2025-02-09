#include "usbEndpoint.h"
#include <string.h>
#include "usbDev.h"

usbHardwareEndpoint::usbHardwareEndpoint(usbEndpoint *parent){
  m_parent=parent;
}

/*!
 * The msg argument is used to pass the number of bytes read or written.
 */
void usbHardwareEndpoint::interrupt(usbHardwareEndpoint::interruptType info, uint16_t msg){
  switch(info){
	case interruptType::dataRx: m_parent->dataRecieved(msg);
  }
}
	
usbEndpoint::usbEndpoint(uint16_t bufferSz, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType){
  bufferSize=bufferSz;
  size=sz;
  dir=uDir;
  type=uType;
  
  inBuffer=nullptr;
  outBuffer=nullptr;
  m_endpointNumber=0x0F;
}

void usbEndpoint::initialise(usbDev *usbHardware){
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
	
void usbEndpoint::setHardwareEndpoint(uint8_t address, usbHardwareEndpoint *hwEp){
  m_endpointNumber=address;
  descriptor.bEndpointAddress=address & 0x0F; // Update descriptor at same time
  hw=hwEp; 
}

uint8_t usbEndpoint::hardwareEndpoint(){
  return m_endpointNumber;
}

void usbEndpoint::dataRecieved(uint16_t nBytes){		
}

uint16_t usbEndpoint::write(uint8_t *data, uint16_t nBytes, uint16_t maxLength){
  uint16_t msgLen=nBytes;
  if (hw==nullptr) return 0;

  if (msgLen>maxLength) msgLen=maxLength;
  if (msgLen>bufferSize) msgLen=bufferSize;

  return hw->write(data, msgLen);
}

void usbEndpoint::writeZLP(){
  if (hw==nullptr) return;
  hw->writeZLP();
}

void usbEndpoint::writeStall(){
  if (hw==nullptr) return;
  hw->writeStall();
}