#include "usbSetupPacket.h"
#include <string.h>

usbSetupPacket::usbSetupPacket(uint8_t *data){
  descriptor=usbDescriptorTypes::invalid;
  descriptorIndex=0;

  memcpy(&bmRequestType, data+0, 1);
  memcpy(&bRequest,      data+1, 1);
  memcpy(&wValue,        data+2, 2); wValue  = fromLE_u16(wValue);
  memcpy(&wIndex,        data+4, 2); wIndex  = fromLE_u16(wIndex);
  memcpy(&wLength,       data+6, 2); wLength = fromLE_u16(wLength);

  if (request==usbStandardRequestCode::getDescriptor){
    descriptor=(usbDescriptorTypes)(wValue >> 8);
    descriptorIndex=wValue & 0xFF;
  }
}

