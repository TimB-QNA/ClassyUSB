#include "usbComponent.h"
#include <string.h>

usbComponent::usbComponent(const char* name){
  m_componentName=name;
}

void usbComponent::initialise(usbDev *usbHardware){
  uint8_t i;
  
  initComponent();

  for (i=0;i<m_nIface;i++){
    m_iface[i]->initialise(usbHardware);
  }
}

void usbComponent::initComponent(){
}

void usbComponent::bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len){
  uint8_t i;  
  
  for (i=0;i<m_nIface;i++) m_iface[i]->bufferInterfaceDescriptor(buffer, len);
}

bool usbComponent::addInterface(usbInterface *iface){
  if (m_nIface==USB_MAX_COMPONENT_INTERFACES) return false;

  m_iface[m_nIface]=iface;
  m_nIface++;

  return true;
}

uint8_t usbComponent::endpoints(){
  uint8_t nEp=0;
  uint8_t i;

  for (i=0;i<m_nIface;i++) nEp+=m_iface[i]->endpoints();
  return nEp;
}

void usbComponent::exec(uint64_t millis){

}

uint8_t usbComponent::assignInterfaceNumbers(uint8_t start){
  uint8_t i;
  uint8_t n=start;
  
  for (i=0;i<m_nIface;i++){
    m_iface[i]->assignInterfaceNumber(n);
    n++;
  }
  return n;
}

/*!
 * Setup packets received here (which come from the main device control pipe)
 * are \emph{always} of type "Class"
 */
bool usbComponent::usbClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  uint8_t i;

  if (pkt.bmRequestType.recipient==usbSetupPacket::rqRecipient::interface){
    for (i=0;i<m_nIface;i++){
      if ((pkt.wIndex & 0xFF)==m_iface[i]->interfaceNumber()){
        if (handleClassRequest(replyEp, pkt)==true) return true;
        return m_iface[i]->handleClassRequest(replyEp, pkt);
      }
    }
  }
  return false;
}

bool usbComponent::usbInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  uint8_t i;

  if (pkt.bmRequestType.recipient==usbSetupPacket::rqRecipient::interface){
    for (i=0;i<m_nIface;i++){
      if ((pkt.wIndex & 0xFF)==m_iface[i]->interfaceNumber()){
        if (handleInterfaceRequest(replyEp, pkt)==true) return true;
        return m_iface[i]->handleInterfaceRequest(replyEp, pkt);
      }
    }
  }
  return false;
}

bool usbComponent::handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  return false;
}

bool usbComponent::handleInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  return false;
}