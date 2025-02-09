#include "usbHID_Joystick.h"
#include "usbHID.h"
#include <string.h>

usbHID_Joystick::channel::channel(usbHID_Joystick::channelLogicalType logicalType, usbHID_Joystick::channelDataType dataType, int32_t logicalMin, int32_t logicalMax, usbHID_Joystick::channelUsage usage){
  m_logicalType=logicalType;
  m_dataType=dataType;
  m_usage=usage;
  m_logicalMin=logicalMin;
  m_logicalMax=logicalMax;
  m_value=0;
}

void usbHID_Joystick::channel::setValue(int32_t val){
  m_value=val;
  parent->update();
}

usbHID_Joystick::usbHID_Joystick() : usbComponent(nullptr, (uint8_t)usbComponent::classCodes::HID, (uint8_t)usbHID::subclassCodes::none, (uint8_t)usbHID::protocolCodes::none, "Joystick"){
  m_nChans=0;
  inEndpoint = new usbHID::inEndpoint();
  assignEndpoint( inEndpoint );
}

bool usbHID_Joystick::addChannel(channel *userChan){
  if (m_nChans==MAX_USB_JOYSTICK_CHANS-1) return false;
  
  userChan->parent=this;
  chan[m_nChans]=userChan;
  m_nChans++;
  return true;
}

void usbHID_Joystick::initComponent(){
 
}

void usbHID_Joystick::bufferSupplementalInterfaceDescriptor(uint8_t *buffer, uint16_t *len){
  usbHID::hidDescriptor hidDesc;
  
  hidDesc.bLength=9;
  hidDesc.bDescriptorType=(uint8_t)usbHID::descriptorCode::HID;
  hidDesc.bcdHID=0x0111;
  hidDesc.bCountryCode=(uint8_t)usbHID::countryCodes::notSupported;
  hidDesc.bNumDescriptors=1;                                                 // Number of descriptors to follow
  hidDesc.bFollowingDescriptorType=(uint8_t)usbHID::descriptorCode::report;  // report type
  hidDesc.bFollowingDescriptorLength=reportLength();                       // report descriptor length
  memcpy(buffer+*len, &hidDesc, hidDesc.bLength); *len+=hidDesc.bLength;
  
//  configurationReport(buffer, len);
}


uint16_t usbHID_Joystick::reportLength(){
//  return 0;
//  return 22;
  return 54;
}


void usbHID_Joystick::configurationReport(uint8_t *buffer, uint16_t *len){
//  uint8_t report[]= { 0x05, 0x01,0x09, 0x04, 0xA1, 0x01, 0x05, 0x02, 0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x01, 0x09, 0x00, 0x82, 0xA2, 0x01, 0xC0 };
  uint8_t report[]= { 0x05, 0x01, 0x09, 0x05, 0xA1, 0x01, 0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x08, 0x15, 0x00, 0x25, 0x01, 0x95, 0x08, 0x75, 0x01, 0x81, 0x02, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x09, 0x32, 0x09, 0x33, 0x09, 0x34, 0x09, 0x35, 0x09, 0x36, 0x09, 0x36, 0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x08, 0x81, 0x02, 0xC0, 0xC0 };
	
	
  memcpy(buffer+*len, report, reportLength());
  *len+=reportLength();
}

/*
void usbHID_Joystick::addReportTag(uint8_t *buffer, uint16_t *len, ){
  
}
*/

void usbHID_Joystick::handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  uint8_t buffer[256];
  uint16_t bufferLen;
  
  if (pkt.bRequest==(uint8_t)usbHID::classRequestCode::getReport){
	configurationReport(buffer, &bufferLen);
	replyEp->write(buffer, bufferLen);
  }
  
  if (pkt.bRequest==(uint8_t)usbHID::classRequestCode::setIdle) replyEp->writeZLP();
}

void usbHID_Joystick::update(){
	// Send data to PC
}