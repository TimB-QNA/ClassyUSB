#include "usbComponent.h"
#include <string.h>

usbComponent::usbComponent(usbSubComponent *subclass, uint8_t classCode, uint8_t subclassCode, uint8_t protocolCode, const char* name){
  m_cmpNameIdx=0;
  m_componentName=name;
  m_subclass=subclass;
  
  usb_ifaceDesc.bLength=9;
  usb_ifaceDesc.bDescriptorType    = (uint8_t)usbDescriptorTypes::interface;
  usb_ifaceDesc.bInterfaceNumber   = 0;
  usb_ifaceDesc.bAlternateSetting  = 0;
  usb_ifaceDesc.bInterfaceClass    = classCode;
  usb_ifaceDesc.bInterfaceSubClass = subclassCode;
  usb_ifaceDesc.bInterfaceProtocol = protocolCode;
  
  if (m_subclass!=nullptr){
    if (subclassCode==0x00) usb_ifaceDesc.bInterfaceSubClass = m_subclass->m_subclassCode;
    if (protocolCode==0x00) usb_ifaceDesc.bInterfaceProtocol = m_subclass->m_protocolCode;
  }
}

void usbComponent::initialise(usbDev *usbHardware){
  uint8_t i;

  usb_ifaceDesc.iInterface=usbHardware->addStringDescriptor(m_componentName);
  
  initComponent();
  if (m_subclass!=nullptr) m_subclass->initialise(this);

  for (i=0;i<m_nEp;i++){
	m_ep[i]->initialise(usbHardware);
  }

  if (m_subclass!=nullptr){
    for (i=0;i<m_subclass->m_nEp;i++){ 
      m_subclass->m_ep[i]->initialise(usbHardware);
    }
  }
}

/*
const uint8_t cdc_acm_interface_descriptor[] = {
	// Interface Descriptor
	0x04, // bDescriptorType: Interface
	0x09, // bInterfaceNumber: Interface number (usually 0)
	0x00, // bAlternateSetting: Alternate setting number (always 0)
	0x01, // bNumEndpoints: Number of endpoints
	0x02, // bInterfaceClass: CDC class code
	0x02, // bInterfaceSubClass: ACM subclass code
	0x01, // bInterfaceProtocol: ACM protocol code
	0x00, // iInterface: String descriptor index for interface

	// CDC Header Functional Descriptor
	0x05, // bDescriptorType: CS_INTERFACE
	0x01, // bDescriptorSubtype: Header Functional Descriptor
	0x01, // bcdCDC: CDC version (1.1)

	// Call Management Functional Descriptor
	0x05, // bDescriptorType: CS_INTERFACE
	0x02, // bDescriptorSubtype: Call Management Functional Descriptor
	0x01, // bmCapabilities: Bitmask for call management features (e.g., call setup, call disconnect)

	// Abstract Control Management Functional Descriptor
	0x05, // bDescriptorType: CS_INTERFACE
	0x02, // bDescriptorSubtype: Abstract Control Management Functional Descriptor
	0x01, // bmCapabilities: Bitmask for ACM features (e.g., line coding, serial state)

	// Data Interface Descriptor
	0x04, // bDescriptorType: Interface
	0x01, // bInterfaceNumber: Data interface number
	0x00, // bAlternateSetting: Alternate setting number (always 0)
	0x02, // bNumEndpoints: Number of endpoints
	0x0A, // bInterfaceClass: Data class code (bulk)
	0x00, // bInterfaceSubClass: Data subclass code
	0x00, // bInterfaceProtocol: Data protocol code
	0x00, // iInterface: String descriptor index for interface

	// Endpoint Descriptor (Bulk IN)
	0x05, // bDescriptorType: Endpoint
	0x01, // bEndpointAddress: Endpoint address (IN)
	0x02, // bmAttributes: Bulk transfer type
	0x00, // wMaxPacketSize: Maximum packet size
	0x00, // bInterval: Polling interval (not used for bulk)

	// Endpoint Descriptor (Bulk OUT)
	0x05, // bDescriptorType: Endpoint
	0x02, // bEndpointAddress: Endpoint address (OUT)
	0x02, // bmAttributes: Bulk transfer type
	0x00, // wMaxPacketSize: Maximum packet size
	0x00, // bInterval: Polling interval (not used for bulk)
};
*/
void usbComponent::bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len){
  uint8_t i;  
  
  // Fill in any descriptor data we didn't have at the constructor stage.
  usb_ifaceDesc.bNumEndpoints      = m_nEp;
  
  // Copy interface and endpoint descriptor to buffer.
  memcpy(buffer+*len, &usb_ifaceDesc, usb_ifaceDesc.bLength); *len+=usb_ifaceDesc.bLength;
  bufferSupplementalInterfaceDescriptor(buffer, len);
     
  for (i=0;i<m_nEp;i++){
	memcpy(buffer+*len, &(m_ep[i]->descriptor), m_ep[i]->descriptor.bLength);
	*len+=m_ep[i]->descriptor.bLength;
  }
  bufferFunctionalDescriptor(buffer, len);
  
  if (m_subclass!=nullptr) m_subclass->bufferInterfaceDescriptor(buffer, len);
  if (m_subclass!=nullptr) bufferFunctionalDescriptor(buffer, len);
}

void usbComponent::bufferSupplementalInterfaceDescriptor(uint8_t *buffer, uint16_t *len){
}

void usbComponent::bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len){
}
	
bool usbComponent::assignEndpoint(usbEndpoint *ep){
  if (m_nEp==USB_MAX_COMPONENT_ENDPOINTS) return false;

  m_ep[m_nEp]=ep;
  m_nEp++;

  return true;
}

uint8_t usbComponent::endpoints(){
  return m_nEp;
}

//usbEndpoint usbComponent::endpoint(uint8_t i){
//}

// Do nothing by default.
void usbComponent::exec(uint64_t millis){

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
  
  if (m_subclass!=nullptr){
	m_subclass->assignInterfaceNumbers(n);
	n++;
  }
  
  return n;
}

/*!
 * Setup packets received here (which come from the main device control pipe)
 * are \emph{always} of type "Class"
 */
bool usbComponent::usbClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  if (pkt.bmRequestType.recipient==usbSetupPacket::rqRecipient::interface){
	if ((pkt.wIndex & 0xFF)==usb_ifaceDesc.bInterfaceNumber){ return handleClassRequest(replyEp, pkt); }
  }
  if (m_subclass!=nullptr) return m_subclass->usbClassRequest(replyEp, pkt);
  return false;
}

/*!
 * Re-implement in your driver and return true if the request was handled.
 */
bool usbComponent::handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  return false;
}


bool usbComponent::usbInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  if (pkt.bmRequestType.recipient==usbSetupPacket::rqRecipient::interface){
	if ((pkt.wIndex & 0xFF)==usb_ifaceDesc.bInterfaceNumber){ return handleInterfaceRequest(replyEp, pkt); }
  }
  if (m_subclass!=nullptr) return m_subclass->handleInterfaceRequest(replyEp, pkt);
  return false;
}

/*!
 * Re-implement in your driver and return true if the request was handled.
 */
bool usbComponent::handleInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  return false;
}
