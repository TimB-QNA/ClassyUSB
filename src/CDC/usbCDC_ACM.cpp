#include "usbCDC_ACM.h"
#include "usbCDC.h"
#include <string.h>

usbCDC_ACM::usbCDC_ACM() : usbSubCDC(0x02, 0x00) // Abstract Control Model, no protocol required
{
  dInEp  = new usbCDC_ACM::cdcAcmEndpoint(this, 128, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::in,  usbEndpoint::endpointType::bulk);
  dOutEp = new usbCDC_ACM::cdcAcmEndpoint(this, 128, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::out,  usbEndpoint::endpointType::bulk);
  assignEndpoint(dInEp);
  assignEndpoint(dOutEp);
  
  sub_ifaceDesc.bInterfaceClass    = 0x0A;
  sub_ifaceDesc.bInterfaceSubClass = 0;
  sub_ifaceDesc.bInterfaceProtocol = 0;
  
  acm_mgtDesc.bFunctionLength=4;
//  acm_mgtDesc.bDescriptorType=;
  acm_mgtDesc.bDescriptorSubtype=0x02;
  acm_mgtDesc.bmCapabilities=0x06;
}

void usbCDC_ACM::initComponent(){

}
	
usbCDC_ACM::cdcAcmEndpoint::cdcAcmEndpoint(usbCDC_ACM *p, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType) : usbEndpoint(bSize, sz,uDir,uType){
  parent=p;
}

void usbCDC_ACM::cdcAcmEndpoint::dataRecieved(uint16_t nBytes){
  // Handle Received data

}

void usbCDC_ACM::bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len){
  usbComponent::headerDescriptor hdrDesc;
  usbCDC::capabilityDescriptor capability;
  usbCDC::functionalDescriptor fncDesc;
 
  hdrDesc.bFunctionLength=5;
  hdrDesc.bDescriptorType=(uint8_t)usbCDC::descriptorTypes::cs_interface;
  hdrDesc.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::headerFunctionalDescriptor;
  hdrDesc.bcdCDC=0x0110;
  memcpy(buffer+*len, &hdrDesc, hdrDesc.bFunctionLength); *len+=hdrDesc.bFunctionLength;

  capability.bFunctionLength=0x04;
  capability.bDescriptorType=(uint8_t)usbCDC::descriptorTypes::cs_interface;
  capability.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::acmFunctional;
  capability.bmCapabilities=0x0F; // Supports everything in ACM.
  memcpy(buffer+*len, &capability, capability.bFunctionLength); *len+=capability.bFunctionLength;

  /*
  fncDesc.bFunctionLength=0x05;
  fncDesc.bDescriptorType=(uint8_t)usbDescriptorTypes::cs_interface;
  fncDesc.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::unionFunctional;
  fncDesc.bFnData[0]=1;               // Control Interface
  fncDesc.bFnData[1]=1;               // Subordinate Interface - Data
  memcpy(buffer+*len, &fncDesc, fncDesc.bFunctionLength); *len+=fncDesc.bFunctionLength;
  
  fncDesc.bFunctionLength=0x05;
  fncDesc.bDescriptorType=(uint8_t)usbDescriptorTypes::cs_interface;
  fncDesc.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::callManagement;
  fncDesc.bFnData[0]=0x03;               // Control Interface
  fncDesc.bFnData[1]=2;                  // Subordinate Interface - Data
  memcpy(buffer+*len, &fncDesc, fncDesc.bFunctionLength); *len+=fncDesc.bFunctionLength;
  
  */
}