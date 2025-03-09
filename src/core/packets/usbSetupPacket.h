#ifndef USBSETUPPACKET_H_
#define USBSETUPPACKET_H_

#include "../usbTypeConversion.h"
#include "../usbTypes.h"
#include <stdint.h>

class usbSetupPacket{
  public:
    enum class rqDir       : uint8_t { toDevice=0x00,    toHost=0x80 };
    enum class rqType      : uint8_t { standard=0x00,  usbClass=0x20,   vendor=0x40, reserved=0x60 };
    enum class rqRecipient : uint8_t {   device=0x00, interface=0x01, endpoint=0x02,    other=0x03 };

    struct{
      uint8_t     data;
      rqDir       dir;
      rqType      type;
      rqRecipient recipient;
    }bmRequestType;
    
    struct{
      uint8_t  data;
      usbStandardRequestCode request;
    }bRequest;
    
    uint16_t wValue;
    
    usbDescriptorTypes descriptor;
    uint8_t descriptorIndex;

    uint16_t wIndex;
    uint16_t wLength;
    
    usbSetupPacket(uint8_t *data);
    uint8_t rawData[8];
};

#endif /* USBSETUPPACKET_H_ */