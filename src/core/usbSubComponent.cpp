#include "usbSubComponent.h"
#include <string.h>

usbSubComponent::usbSubComponent(uint8_t subclassCode, uint8_t protocolCode){
  m_subclassCode=subclassCode;
  m_protocolCode=protocolCode;
  
  sub_ifaceDesc.bLength=9;
  sub_ifaceDesc.bDescriptorType    = (uint8_t)usbDescriptorTypes::interface;
  sub_ifaceDesc.bInterfaceNumber   = 0;
  sub_ifaceDesc.bAlternateSetting  = 0;
  sub_ifaceDesc.bInterfaceClass    = 0;
  sub_ifaceDesc.bInterfaceSubClass = 0;
  sub_ifaceDesc.bInterfaceProtocol = 0;
  sub_ifaceDesc.bNumEndpoints      = 0;
}

void usbSubComponent::initialise(usbComponent *parent){
  m_parent=parent;
  initComponent();
}
	
bool usbSubComponent::assignEndpoint(usbEndpoint *ep){
  if (m_nEp==USB_MAX_COMPONENT_ENDPOINTS) return false;
  m_ep[m_nEp]=ep;
  m_nEp++;
  return true;
}

void usbSubComponent::bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len){
  uint8_t i;
  
  // Fill in any descriptor data we didn't have at the constructor stage.
  sub_ifaceDesc.bNumEndpoints      = m_nEp;

  // Copy interface and endpoint descriptor to buffer.
  memcpy(buffer+*len, &sub_ifaceDesc, sub_ifaceDesc.bLength); *len+=sub_ifaceDesc.bLength;

  bufferFunctionalDescriptor(buffer, len);

  for (i=0;i<m_nEp;i++){
	memcpy(buffer+*len, &(m_ep[i]->descriptor), m_ep[i]->descriptor.bLength); *len+=m_ep[i]->descriptor.bLength;
  }
}

void usbSubComponent::bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len){
//	if (m_subclass!=nullptr) m_subclass->bufferFunctionalDescriptor(buffer, len);
}

uint8_t usbSubComponent::assignInterfaceNumbers(uint8_t start){
	uint8_t n=start;
	sub_ifaceDesc.bInterfaceNumber = n; n++;
	
	return n;
}

uint8_t usbSubComponent::endpoints(){
	return m_nEp;
}

/*!
 * Setup packets received here (which come from the main device control pipe)
 * are \emph{always} of type "Class"
 */
bool usbSubComponent::usbClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  if (pkt.bmRequestType.recipient==usbSetupPacket::rqRecipient::interface){
	if ((pkt.wIndex & 0xFF)==sub_ifaceDesc.bInterfaceNumber){ return handleClassRequest(replyEp, pkt); }
  }
  return false;
}

bool usbSubComponent::handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  return false;
}

bool usbSubComponent::usbInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  if (pkt.bmRequestType.recipient==usbSetupPacket::rqRecipient::interface){
	if ((pkt.wIndex & 0xFF)==sub_ifaceDesc.bInterfaceNumber){ return handleInterfaceRequest(replyEp, pkt); }
  }
  return false;
}

bool usbSubComponent::handleInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt){	
  return false;
}