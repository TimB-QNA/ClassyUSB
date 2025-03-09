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

usbHID_Joystick::usbHID_Joystick() : usbComponent("Joystick"){
  m_nChans=0;

  iface.hidConfig=&hidConfig;
  inEndpoint = new usbHID::inEndpoint();
  iface.addEndpoint( inEndpoint );

  addInterface(&iface);
}

bool usbHID_Joystick::addChannel(channel *userChan){
  if (m_nChans==MAX_USB_JOYSTICK_CHANS-1) return false;
  
  userChan->parent=this;
  chan[m_nChans]=userChan;
  m_nChans++;
  return true;
}

void usbHID_Joystick::exec(uint64_t millis){
}

bool usbHID_Joystick::handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  uint8_t buffer[256];
  uint16_t bufferLen;
  
  if (pkt.bmRequestType.data!=0x21 && pkt.bmRequestType.data!=0xA1) return false; // Only two acceptable packet types.
  
  if (pkt.bRequest.data==(uint8_t)usbHID::classRequestCode::getReport){
    hidConfig.writeReport(buffer, &bufferLen);
    replyEp->write(buffer, bufferLen);
  }
  
  if (pkt.bRequest.data==(uint8_t)usbHID::classRequestCode::setIdle) replyEp->writeZLP();
  
  return true;
}

bool usbHID_Joystick::handleInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt){
  uint8_t buffer[256];
  uint16_t bufferLen;
  
  if (pkt.bRequest.request==usbStandardRequestCode::getDescriptor){
    if ((uint16_t)pkt.descriptor==(uint16_t)usbHID::descriptorCode::report){
        hidConfig.writeReport(buffer, &bufferLen);
      replyEp->write(buffer, bufferLen);
      return true;
    }
  }
  return true;
}


void usbHID_Joystick::update(){
    // Send data to PC
}

usbHID_Joystick::hidJoystickInterface::hidJoystickInterface() : usbInterface((uint8_t)usbComponent::classCodes::HID, (uint8_t)usbHID::subclassCodes::none, (uint8_t)usbHID::protocolCodes::none, 0, "Joystick")
{
}

void usbHID_Joystick::hidJoystickInterface::preEndpointDescriptor(uint8_t *buffer, uint16_t *len){
  usbHID::hidDescriptor hidDesc;
  
  hidDesc.bLength=9;
  hidDesc.bDescriptorType=(uint8_t)usbHID::descriptorCode::HID;
  hidDesc.bcdHID=0x0111;
  hidDesc.bCountryCode=(uint8_t)usbHID::countryCodes::notSupported;
  hidDesc.bNumDescriptors=1;                                                 // Number of descriptors to follow
  hidDesc.bFollowingDescriptorType=(uint8_t)usbHID::descriptorCode::report;  // report type
  hidDesc.bFollowingDescriptorLength=hidConfig->reportLength();              // report descriptor length

  memcpy(buffer+*len, &hidDesc, hidDesc.bLength); *len+=hidDesc.bLength;
}

void usbHID_Joystick::hidJoystickInterface::postEndpointDescriptor(uint8_t *buffer, uint16_t *len){

}