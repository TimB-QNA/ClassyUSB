#ifndef USBSETUPPACKET_H_
#define USBSETUPPACKET_H_

#include "../usbTypeConversion.h"
#include "../usbTypes.h"
#include <stdint.h>

class usbSetupPacket{
  public:
    enum rqDir       { toDevice=0x00,    toHost=0x01 };
    enum rqType      { standard=0x00,  usbClass=0x01,   vendor=0x02, reserved=0x03 };
    enum rqRecipient {   device=0x00, interface=0x01, endpoint=0x02,    other=0x03 };

    union{
      uint8_t bmRequestType;
	  rqDir   reqDir  : 1;
	  rqType  reqType : 2;
	  rqRecipient rec : 5;
	};
	
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