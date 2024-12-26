#ifndef USBSETUPPACKET_H_
#define USBSETUPPACKET_H_

#include "../usbTypeConversion.h"
#include "../usbTypes.h"
#include <stdint.h>

class usbSetupPacket{
  public:
    uint8_t bmRequestType;
    union{
      uint8_t  bRequest;
      usbStandardRequestCode request;
    };
    uint16_t wValue;
    
    usbDescriptorTypes descriptor;
    uint8_t descriptorIndex;

    uint16_t wIndex;
    uint16_t wLength;
    
    usbSetupPacket(uint8_t *data);
};

#endif /* USBSETUPPACKET_H_ */