#include "usbCDC.h"
#include <string.h>

usbCDC::usbCDC(usbComponent *subclass, const char* name) : usbComponent(0x02, 0x00, subclass, name){
  ctrlEndpoint = new cdcEndpoint(this, 64, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::in, usbEndpoint::endpointType::interrupt);
  assignEndpoint( ctrlEndpoint );
}

void usbCDC::bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len){
  usbCDC::headerDescriptor header;

  header.bFunctionLength=5;
  header.bDescriptorType=(uint8_t)usbDescriptorTypes::cs_interface;
  header.bDescriptorSubtype=0x00;
  header.bcdCDC=0x0110;

  memcpy(buffer+*len, &header, header.bFunctionLength); *len+=header.bFunctionLength;

  usbCDC::functionalDescriptor ctrl;

  ctrl.bFunctionLength=0x05;
  ctrl.bDescriptorType=(uint8_t)usbDescriptorTypes::cs_interface;
  ctrl.bDescriptorSubtype=0x06; //
  ctrl.bControlInterface=ctrlEndpoint->descriptor.bEndpointAddress;
  ctrl.bSubordinateInterface0=m_subclass->usb_ifaceDesc.bInterfaceNumber;

  memcpy(buffer+*len, &ctrl, ctrl.bFunctionLength); *len+=ctrl.bFunctionLength;
  m_subclass->bufferFunctionalDescriptor(buffer, len);
}

usbCDC::cdcEndpoint::cdcEndpoint(usbCDC *p, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType) : usbEndpoint(bSize, sz, uDir, uType){
  parent=p;
}

void usbCDC::cdcEndpoint::dataRecieved(uint16_t nBytes){
  // Handle Received data

}