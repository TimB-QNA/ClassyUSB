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

class usbCDC : public usbComponent
{
  public:
    enum class subclassCodes        : uint8_t  { reserved                   = 0x00,
                                                 directLineControlModel     = 0x01,
                                                 abstractControlModel       = 0x02,
                                                 telephoneControlModel      = 0x03,
                                                 multichannelControlModel   = 0x04,
                                                 capiControlModel           = 0x05,
                                                 ethernetNetworking         = 0x06,
                                                 atmControlModel            = 0x07,
                                                 wirelessHandset            = 0x08,
                                                 deviceManagement           = 0x09,
                                                 mobileDirectLine           = 0x0A,
                                                 obex                       = 0x0B };
                                                 
    enum class descriptorTypes      : uint16_t { cs_interface               = 0x24,
                                                 cs_endpoint                = 0x25 };

    enum class descriptorSubTypes   : uint16_t { headerFunctionalDescriptor = 0x00,
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
    #pragma pack(push, 1)
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

    typedef struct{
      uint8_t  bFunctionLength;
      uint8_t  bDescriptorType;
      uint8_t  bDescriptorSubtype;
      uint16_t bcdCDC;
    }headerDescriptor;
    #pragma pack(pop)
            
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
    usbCDC(const char* name="CDC");

  protected:
    usbEndpoint *ctrlEndpoint;
};

#endif /* USBCDC_H_ */