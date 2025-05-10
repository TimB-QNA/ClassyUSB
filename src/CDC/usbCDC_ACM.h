/*
 * usbCDC_ACM.h
 *
 * Created: 13/08/2024 18:48:03
 *  Author: Tim
 */ 

#ifndef USBCDC_ACM_H_
#define USBCDC_ACM_H_

#include "usbCDC.h"
#include "../core/usbInterface.h"
#include "../utils/ringBuffer.h"
#include <stdio.h>

class usbCDC_ACM : public usbCDC
{
  friend class usbCDC;
  friend int writeCdcAcm(void *cookie, const char *buf, int n);
  friend int readCdcAcm(void *cookie, char *buf, int n);

  private:
    class cdcAcmEndpoint : public usbEndpoint{
      public:
        enum class opMode : uint8_t { alert = 0x00, receive = 0x01, transmit = 0x02 };
        cdcAcmEndpoint(usbCDC_ACM *p, usbCDC_ACM::cdcAcmEndpoint::opMode mode, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType);
      
      protected:
        void dataRecieved(uint16_t nBytes);
        void transmitComplete();
      
      private:
        usbCDC_ACM::cdcAcmEndpoint::opMode m_mode;
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
    usbCDC_ACM(uint8_t *txBuffSpace, uint16_t txSize, uint8_t *rxBuffSpace, uint16_t rxSize);

    uint16_t write(uint8_t *data, uint16_t nBytes);

    FILE *stream;

  protected:
    typedef struct{
      uint8_t  bFunctionLength;
      uint8_t  bDescriptorType;
      uint8_t  bDescriptorSubtype;
      uint8_t  bmCapabilities;
    }managementDescriptor;

    void initComponent();
    managementDescriptor acm_mgtDesc;

    cdcMainInterface           m_mainIface;
    cdcDataInterface           m_dataIface;
    usbCDC_ACM::cdcAcmEndpoint *dInEp;
    usbCDC_ACM::cdcAcmEndpoint *dOutEp;
    usbCDC_ACM::cdcAcmEndpoint *ctrlEp;

    ringBuffer rxBuffer;
    ringBuffer txBuffer;
};

#endif /* USBCDC_ACM_H_ */