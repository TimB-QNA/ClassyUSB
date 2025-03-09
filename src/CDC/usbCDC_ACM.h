/*
 * usbCDC_ACM.h
 *
 * Created: 13/08/2024 18:48:03
 *  Author: Tim
 */ 

#include "usbCDC.h"
#include "../core/usbInterface.h"

#ifndef USBCDC_ACM_H_
#define USBCDC_ACM_H_

class usbCDC_ACM : public usbCDC
{
  friend class usbCDC;
  
  private:
    class cdcAcmEndpoint : public usbEndpoint{
      public:
        cdcAcmEndpoint(usbCDC_ACM *p, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType);
        void dataRecieved(uint16_t nBytes);

      private:
        usbCDC_ACM *parent;
    };
    
    class cdcMainInterface : public usbInterface{
      public:
        cdcMainInterface();
        usbInterface *dataInterface;

      protected:  
        void preEndpointDescriptor(uint8_t *buffer, uint16_t *len);
    };

    class cdcDataInterface : public usbInterface{
      public:
        cdcDataInterface();
    };

  friend class cdcAcmEndpoint;

  public:
    typedef struct{
      uint8_t  bFunctionLength;
      uint8_t  bDescriptorType;
      uint8_t  bDescriptorSubtype;
      uint8_t  bmCapabilities;
    }managementDescriptor;

    usbCDC_ACM();

    uint16_t write(uint8_t *data, uint16_t nBytes);

  protected:
    void initComponent();
    managementDescriptor acm_mgtDesc;

    cdcMainInterface           m_mainIface;
    cdcDataInterface           m_dataIface;
    usbCDC_ACM::cdcAcmEndpoint *dInEp;
    usbCDC_ACM::cdcAcmEndpoint *dOutEp;
    usbCDC_ACM::cdcAcmEndpoint *ctrlEp;
};

#endif /* USBCDC_ACM_H_ */