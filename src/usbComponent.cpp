#include "usbComponent.h"
#include <string.h>

usbComponent::usbComponent(uint8_t classCode, uint8_t classControlProtocol, usbComponent *subclass, const char* name){
  m_cmpNameIdx=0;
  m_componentName=name;
  m_subclass=subclass;

  usb_ifaceDesc.bLength=9;
  usb_ifaceDesc.bDescriptorType    = (uint8_t)usbDescriptorTypes::interface;
  usb_ifaceDesc.bInterfaceClass    = classCode;
  usb_ifaceDesc.bInterfaceSubClass = 0x00;
  if (m_subclass!=nullptr) usb_ifaceDesc.bInterfaceSubClass = m_subclass->subclassCode();
  usb_ifaceDesc.bInterfaceProtocol = classControlProtocol;
  if (classControlProtocol==0 && m_subclass!=nullptr) usb_ifaceDesc.bInterfaceProtocol = m_subclass->protocolCode();
  usb_ifaceDesc.bNumEndpoints=0;
}

void usbComponent::updateInterfaceDescriptor(){
// Don't update anything by default
}

void usbComponent::bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len){
  uint8_t i;
  
  // Apply user-level changes.
  updateInterfaceDescriptor();

  // Fill in any descriptor data we didn't have at the constructor stage.
  usb_ifaceDesc.bAlternateSetting  = 0;
  usb_ifaceDesc.bNumEndpoints      = m_nEp;
  usb_ifaceDesc.iInterface         = m_cmpNameIdx;
  
  // Copy interface and endpoint descriptor to buffer.
  memcpy(buffer+*len, &usb_ifaceDesc, usb_ifaceDesc.bLength); *len+=usb_ifaceDesc.bLength;

  for (i=0;i<m_nEp;i++){
    memcpy(buffer+*len, &(m_ep[i]->descriptor), m_ep[i]->descriptor.bLength); *len+=m_ep[i]->descriptor.bLength;
  }

  if (m_subclass!=nullptr) m_subclass->bufferInterfaceDescriptor(buffer, len);

  bufferFunctionalDescriptor(buffer, len);
}

void usbComponent::bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len){
  if (m_subclass!=nullptr) m_subclass->bufferFunctionalDescriptor(buffer, len);
}

bool usbComponent::assignEndpoint(usbEndpoint *ep){
  if (m_nEp==USB_MAX_COMPONENT_ENDPOINTS) return false;

  m_ep[m_nEp]=ep;
  m_nEp++;

  return true;
}

uint8_t usbComponent::endpoints(){
  return m_nEp+m_subclass->endpoints();
}

//usbEndpoint usbComponent::endpoint(uint8_t i){
//}

// Do nothing by default.
void usbComponent::exec(){

}

uint8_t usbComponent::subclassCode(){
  return 0;
}

uint8_t usbComponent::protocolCode(){
  return 0;
}

uint8_t usbComponent::assignInterfaceNumbers(uint8_t start){
  uint8_t n=start;
  usb_ifaceDesc.bInterfaceNumber = n; n++;
  if (m_subclass!=nullptr) n=m_subclass->assignInterfaceNumbers(n);
  return n;
}