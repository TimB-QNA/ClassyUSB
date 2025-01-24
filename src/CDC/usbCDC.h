/*
 * usbCDC.h
 *
 * Created: 13/08/2024 18:41:26
 *  Author: Tim
 */ 


#ifndef USBCDC_H_
#define USBCDC_H_

#include <stdint.h>
#include "../usbComponent.h"
#include "../usbSubComponent.h"

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
  private:
	enum class descriptorTypes         : uint16_t { interface=0x24, endpoint=0x25 };
	enum class descriptorSubTypes      : uint16_t { headerFunctionalDescriptor=0x00, callManagement=0x02 };
		
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
    usbComponent::headerDescriptor      cdc_hdrDesc;
    usbComponent::functionalDescriptor  cdc_fncDesc;
    usbEndpoint                        *ctrlEndpoint;

    void initComponent();

    void bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len);
};

#endif /* USBCDC_H_ */