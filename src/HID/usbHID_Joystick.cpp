#include "usbHID_Joystick.h"
#include "usbHID.h"
#include <string.h>

usbHID_Joystick::usbHID_Joystick(uint8_t axes, uint8_t buttons) : usbComponent(nullptr, 0x03, (uint8_t)usbHID::subclassCodes::none, (uint8_t)usbHID::protocolCodes::none, "Joystick"){
  inEndpoint = new usbHID::inEndpoint();
  assignEndpoint( inEndpoint );
}
    
void usbHID_Joystick::initComponent(){
}

void usbHID_Joystick::bufferSupplementalInterfaceDescriptor(uint8_t *buffer, uint16_t *len){
  usbHID::hidDescriptor hidDesc;
  
  hidDesc.bLength=7;
  hidDesc.bDescriptorType=(uint8_t)usbHID::descriptorCode::HID;
  hidDesc.bcdHID=0x0101;
  hidDesc.bCountryCode=(uint8_t)usbHID::countryCodes::notSupported;
  hidDesc.bNumDescriptors=1;     // Number of descriptors to follow
  hidDesc.bFollowingDescriptorType=(uint8_t)usbHID::descriptorCode::report;     // report type
  hidDesc.bFollowingDescriptorLength=0;   // report descriptor length
  memcpy(buffer+*len, &hidDesc, hidDesc.bLength); *len+=hidDesc.bLength;
}

uint8_t usbHID_Joystick::reportLength(){
}

void usbHID_Joystick::configurationReport(uint8_t *buffer, uint16_t *len){
  
}

void usbHID_Joystick::addReportTag(uint8_t *buffer, uint16_t *len, ){
  
}

void usbHID_Joystick::handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
}

