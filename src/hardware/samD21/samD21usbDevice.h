/*
 * samd21usbDevice.h
 *
 * Created: 13/08/2024 18:02:49
 *  Author: Tim
 */

#define SAMUSB21_MAX_EPS 8

#ifndef SAMD21USBDEVICE_H_
#define SAMD21USBDEVICE_H_
#include <stdalign.h>
#include <sam.h>
#include "../../core/usbDev.h"
#include "../../core/usbEndpoint.h"

extern "C" void USB_Handler();

class samD21usbEndpoint : public usbHardwareEndpoint
{
    public:
    samD21usbEndpoint(usbEndpoint *parent, uint8_t epIdx);
    uint8_t sizeCode(usbEndpoint::endpointSize sz);
    uint8_t typeCode(usbEndpoint::endpointType type);
    void configure();

    uint16_t write(uint8_t *data, uint16_t nBytes);
    void writeZLP();
    void writeStall();
        
    void readComplete();
    
    private:
    UsbDeviceEndpoint  *ep=nullptr;
    UsbDeviceDescBank  *out=nullptr;
    UsbDeviceDescBank  *in=nullptr;
};


class samd21usbDevice : public usbDev
{
  friend void USB_Handler();

  public: 
    samd21usbDevice();

    void attach();

  protected:
    void initialiseHardware();
    void hardwareExec(uint64_t millis);
    void setAddress(uint8_t addr);
    void setMode(usbModeTypes mode);

    // Endpoint buffers
    bool allocateEndpointBuffer(uint8_t **buffer, uint16_t bufferSize);
    
  private:
    void configureClock();
    void configurePins();
    bool addEndpointToHardware(uint8_t idx, usbEndpoint *ep);
    void readComplete(uint8_t dEp, uint8_t bnk);

    void interruptHandler();
    void configureEndpoint(uint8_t idx);

    samD21usbEndpoint *epMgt[SAMUSB21_MAX_EPS]; // Pointers to hardware endpoints to simplify configuration after USB Reset.
};

#endif /* SAMD21USBDEVICE_H_ */
