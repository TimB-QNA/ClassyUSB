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

class usbCDC : public usbComponent
{
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
    typedef struct{
      uint8_t  bFunctionLength;
      uint8_t  bDescriptorType;
      uint8_t  bDescriptorSubtype;
      uint16_t bcdCDC;
    }headerDescriptor;

    typedef struct{
      uint8_t  bFunctionLength;
      uint8_t  bDescriptorType;
      uint8_t  bDescriptorSubtype;
      uint8_t  bControlInterface;
      uint8_t  bSubordinateInterface0;
      uint8_t  bInterfaceClass;
      uint8_t  bInterfaceSubClass;
      uint8_t  bInterfaceProtocol;
    }functionalDescriptor;

    typedef struct{
      uint8_t  bFunctionLength;
      uint8_t  bDescriptorType;
      uint8_t  bDescriptorSubtype;
      uint8_t  bmCapabilities;
      uint8_t  bDataInterface;
    }capabilityDescriptor;

    usbCDC(usbComponent *subclass, const char* name="CDC");

  protected:
    headerDescriptor     cdc_hdrDesc;
    functionalDescriptor cdc_fncDesc;

    usbEndpoint *ctrlEndpoint;

    void bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len);
};

#endif /* USBCDC_H_ */