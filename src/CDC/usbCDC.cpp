#include "usbCDC.h"
#include <string.h>

usbCDC::usbCDC(usbSubCDC *subclass, const char* name) : usbComponent((usbSubComponent*)subclass, 0x02, 0x00, 0x00, name){
  ctrlEndpoint = new cdcEndpoint(this, 64, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::in, usbEndpoint::endpointType::interrupt);
  assignEndpoint( ctrlEndpoint );
}
    
void usbCDC::initComponent(){
}

/*
	// CDC Header Functional Descriptor
	0x05, // bDescriptorType: CS_INTERFACE
	0x01, // bDescriptorSubtype: Header Functional Descriptor
	0x01, // bcdCDC: CDC version (1.1)

	// Call Management Functional Descriptor
	0x05, // bDescriptorType: CS_INTERFACE
	0x02, // bDescriptorSubtype: Call Management Functional Descriptor
	0x01, // bmCapabilities: Bitmask for call management features (e.g., call setup, call disconnect)
*/

void usbCDC::bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len){
  usbComponent::headerDescriptor     hdrDesc;
  usbCDC::functionalDescriptor fncDesc;

  hdrDesc.bFunctionLength=5;
  hdrDesc.bDescriptorType=0x05; //(uint8_t)usbCDC::descriptorTypes::interface;
  hdrDesc.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::headerFunctionalDescriptor; 
  hdrDesc.bcdCDC=0x0110;
//  memcpy(buffer+*len, &hdrDesc, hdrDesc.bFunctionLength); *len+=hdrDesc.bFunctionLength;

/*
  usbComponent::functionalDescriptor ctrl;

  ctrl.bFunctionLength=5;
  ctrl.bDescriptorType=(uint8_t)usbCDC::descriptorTypes::interface;
  ctrl.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::callManagement;
  ctrl.bControlInterface=ctrlEndpoint->descriptor.bEndpointAddress;
//  ctrl.bSubordinateInterface0=m_subclass->usb_ifaceDesc.bInterfaceNumber;

  memcpy(buffer+*len, &ctrl, ctrl.bFunctionLength); *len+=ctrl.bFunctionLength;
  */
}

usbCDC::cdcEndpoint::cdcEndpoint(usbCDC *p, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType) : usbEndpoint(bSize, sz, uDir, uType){
  parent=p;
}

void usbCDC::cdcEndpoint::dataRecieved(uint16_t nBytes){
  // Handle Received data

}


usbSubCDC::usbSubCDC(uint8_t subclassCode, uint8_t protocolCode) : usbSubComponent(subclassCode, protocolCode){
	
}