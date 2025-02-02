/*
 * usbCDC.h
 *
 * Created: 13/08/2024 18:41:26
 *  Author: Tim
 */ 


#ifndef USBCDC_H_
#define USBCDC_H_

#include <stdint.h>
#include "../core/usbComponent.h"
#include "../core/usbSubComponent.h"

/*! The usbSubCDC class is just a wrapper around usbSubComponent.
 *  It restricts which object types can be compiled into a USB
 *  Component without a compiler error.
 */
class usbSubCDC : public usbSubComponent
{
  public:
    usbSubCDC(uint8_t subclassCode=0x00, uint8_t protocolCode=0x00);
};

class usbCDC : public usbComponent
{
  public:
    enum class descriptorTypes      : uint16_t { cs_interface = 0x24, cs_endpoint = 0x25 };

    enum class descriptorSubTypes      : uint16_t { headerFunctionalDescriptor = 0x00,
		                                            callManagement             = 0x01,
													acmFunctional              = 0x02,
													directLineManagement       = 0x03,
													phoneRinger                = 0x04,
													phoneCallAndState          = 0x05,
													unionFunctional            = 0x06,
													countrySelection           = 0x07,
													phoneOpModes               = 0x08,
													usbTerminal                = 0x09,
													netChanTerminal            = 0x0A,
													protocolUnit               = 0x0B,
													extensionUnit              = 0x0C,
													multichanManagement        = 0x0D,
													capiControlManagement      = 0x0E,
													ethernetFunctional         = 0x0F,
													atmNetworking              = 0x10,
													wirelessHandsetControl     = 0x11 };
    typedef struct{
	  uint8_t  bFunctionLength;
	  uint8_t  bDescriptorType;
	  uint8_t  bDescriptorSubtype;
	  uint8_t  bmCapabilities;
	  uint8_t  bDataInterface;
    }capabilityDescriptor;

    typedef struct{
      uint8_t  bFunctionLength;
      uint8_t  bDescriptorType;
      uint8_t  bDescriptorSubtype;
      uint8_t  bFnData[6];
    }functionalDescriptor;
		
  private:
    class cdcEndpoint : public usbEndpoint{
      public:
      cdcEndpoint(usbCDC *p, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType);
      void dataRecieved(uint16_t nBytes);

      private:
      usbCDC *parent;
    };

    friend class cdcAcmEndpoint;

  public:
    usbCDC(usbSubCDC *subclass, const char* name="CDC");

  protected:
    usbEndpoint                        *ctrlEndpoint;

    void initComponent();

    void bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len);
};

#endif /* USBCDC_H_ */