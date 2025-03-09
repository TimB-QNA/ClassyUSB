#include "usbInterface.h"
#include "usbDev.h"
#include <string.h>

usbInterface::usbInterface(uint8_t interfaceClass, uint8_t interfaceSubClass, uint8_t interfaceProtocol, uint8_t alternateSetting, const char *iFaceName){
  stdDescriptor.bLength            = 9;
  stdDescriptor.bDescriptorType    = (uint8_t)usbDescriptorTypes::interface;
  stdDescriptor.bInterfaceNumber   = 0xFF;
  stdDescriptor.bAlternateSetting  = alternateSetting;
  stdDescriptor.bNumEndpoints      = 0;
  stdDescriptor.bInterfaceClass    = interfaceClass;
  stdDescriptor.bInterfaceSubClass = interfaceSubClass;
  stdDescriptor.bInterfaceProtocol = interfaceProtocol;
  stdDescriptor.iInterface         = 0;
  m_ifaceName = iFaceName;
}

void usbInterface::initialise(usbDev *usbHardware){
  uint8_t i;

  stdDescriptor.iInterface=usbHardware->addStringDescriptor(m_ifaceName);
  
  for (i=0;i<m_nEp;i++){
    m_ep[i]->initialise(usbHardware);
  }
}

void usbInterface::initInterface(){
}

bool usbInterface::addEndpoint(usbEndpoint *ep){
  if (m_nEp==USB_MAX_INTERFACE_ENDPOINTS) return false;
  m_ep[m_nEp]=ep;
  m_nEp++;
  return true;
}

void usbInterface::bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len){
  uint8_t i;
  
  // Fill in any descriptor data we didn't have at the constructor stage.
  stdDescriptor.bNumEndpoints      = m_nEp;

  // Copy interface and endpoint descriptor to buffer.
  memcpy(buffer+*len, &stdDescriptor, stdDescriptor.bLength); *len+=stdDescriptor.bLength;

  preEndpointDescriptor(buffer, len);

  for (i=0;i<m_nEp;i++){
    memcpy(buffer+*len, &(m_ep[i]->descriptor), m_ep[i]->descriptor.bLength); *len+=m_ep[i]->descriptor.bLength;
  }
  postEndpointDescriptor(buffer, len);
}

void usbInterface::preEndpointDescriptor(uint8_t *buffer, uint16_t *len){
}

void usbInterface::postEndpointDescriptor(uint8_t *buffer, uint16_t *len){
}

bool usbInterface::handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  return false;
}

bool usbInterface::handleInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  return false;
}

void usbInterface::assignInterfaceNumber(uint8_t idx){
  stdDescriptor.bInterfaceNumber = idx;
}

uint8_t usbInterface::interfaceNumber(){
  return stdDescriptor.bInterfaceNumber;
}

uint8_t usbInterface::endpoints(){
  return m_nEp;
}
