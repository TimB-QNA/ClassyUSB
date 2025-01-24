/*
 * usbEndpoint.h
 *
 * Created: 01/09/2024 15:06:04
 *  Author: Tim
 */ 

#ifndef USBENDPOINT_H_
#define USBENDPOINT_H_

#include <stdint.h>
#include "usbTypes.h"

class usbDev;

class usbEndpoint
{
  public:
    typedef struct{
      uint8_t  bLength;             // Size of descriptor (always 7)
      uint8_t  bDescriptorType;     // Interface descriptor Type  - USB Standard page 251
      uint8_t  bEndpointAddress;    // Hardware endpoint location.
      uint8_t  bmAttributes;        //
      uint16_t wMaxPacketSize;      // Number of endpoints used by component
      uint8_t  bInterval;           // Component class
    }endpointDescriptor;
    
    enum class endpointSize      : uint16_t { b8=8, b16=16, b32=32, b64=64, b128=128, b256=256, b512=512, b1023=1023 };
    enum class endpointDirection : uint8_t { none=0x00, in=0x01, out=0x02, both=0x03 };
    enum class endpointType      : uint8_t { control=0x00, isochronos=0x01, bulk=0x02, interrupt=0x03, dual=0x04 };

    usbEndpoint(uint16_t bufferSz, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType);
    void setHardware(usbDev *hw);
	void initialise();
    void setHardwareEndpoint(uint8_t hardwareEndpoint);
    uint8_t hardwareEndpoint();

  // Information used by hardware
    usbEndpoint::endpointSize       size; // Packet Size
    usbEndpoint::endpointDirection  dir;
    usbEndpoint::endpointType       type;
    uint16_t                        bufferSize;
    uint8_t*                        inBuffer;
    uint8_t*                        outBuffer;
    usbDev                          *usbHardware;
	

  // Data Handler routines
    virtual void dataRecieved(uint16_t nBytes)=0;
    virtual void setupRecieved(uint16_t nBytes);

  // Normal IO Functions 
    uint16_t writeIn(uint8_t *data, uint16_t nBytes, uint16_t maxLength=0xFFFF);
    void writeInZLP();
    void writeInStall();

    endpointDescriptor descriptor;
	
  protected:
    uint8_t m_endpointNumber;
};

#endif /* USBENDPOINT_H_ */