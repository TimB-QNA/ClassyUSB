#include "usbCDC_ACM.h"
#include "usbCDC.h"
#include <string.h>

usbCDC_ACM::usbCDC_ACM() : usbComponent()
{
  dInEp  = new usbCDC_ACM::cdcAcmEndpoint(this, 128, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::in,  usbEndpoint::endpointType::bulk);
  dOutEp = new usbCDC_ACM::cdcAcmEndpoint(this, 128, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::out,  usbEndpoint::endpointType::bulk);
  assignEndpoint(dInEp);
  assignEndpoint(dOutEp);
  
  acm_mgtDesc.bFunctionLength=4;
//  acm_mgtDesc.bDescriptorType=;
  acm_mgtDesc.bDescriptorSubtype=0x02;
  acm_mgtDesc.bmCapabilities=0x06;
}

usbCDC_ACM::cdcAcmEndpoint::cdcAcmEndpoint(usbCDC_ACM *p, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType) : usbEndpoint(bSize, sz,uDir,uType){
  parent=p;
}

void usbCDC_ACM::cdcAcmEndpoint::dataRecieved(uint16_t nBytes){
  // Handle Received data

}

void usbCDC_ACM::updateInterfaceDescriptor(){
  usb_ifaceDesc.bInterfaceClass=10;
}

uint8_t usbCDC_ACM::subclassCode(){
  return 2;
}

void usbCDC_ACM::bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len){
  usbCDC::capabilityDescriptor capability;

  capability.bFunctionLength=0x04;
  capability.bDescriptorType=(uint8_t)usbDescriptorTypes::cs_interface;
  capability.bDescriptorSubtype=0x02; //ACM Management;
  capability.bmCapabilities=0x0F; // Supports everything in ACM.

  memcpy(buffer+*len, &capability, capability.bFunctionLength); *len+=capability.bFunctionLength;
}