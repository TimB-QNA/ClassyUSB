/*
 * samd21usbDevice.h
 *
 * Created: 13/08/2024 18:02:49
 *  Author: Tim
 */

 #ifndef SAMUSB21_MAX_EPS
   #define SAMUSB21_MAX_EPS 8
#endif

#ifndef SAMD21USBDEVICE_H_
#define SAMD21USBDEVICE_H_
#include <stdalign.h>
#include <sam.h>
#include "../../usbDev.h"

extern "C" void USB_Handler();

class samd21usbDevice : public usbDev
{
  friend void USB_Handler();

  public: 
    samd21usbDevice();

    void attach();

  protected:
    void initialiseHardware();
    void hardwareExec();
    void setAddress(uint8_t addr);
    void setMode(usbModeTypes mode);
    void sendRequestError(usbEndpoint *ep, uint8_t bnk=0);

    uint16_t writeIn(uint8_t ep, uint16_t nBytes);
    void     writeInZLP(uint8_t ep);
    void     writeInStall(uint8_t ep);

    // Endpoint buffers
    bool allocateEndpointBuffer(uint8_t **buffer, uint16_t bufferSize);
	
  private:
    void configureClock();
    void configurePins();
    uint8_t epSizeCode(usbEndpoint::endpointSize sz);
    bool addEndpointToHardware(uint8_t idx, usbEndpoint *ep);
    void readComplete(uint8_t dEp, uint8_t bnk);

    void interruptHandler();
    void configureEndpoint(uint8_t idx);

    // uint8_t nEP;
};

#endif /* SAMD21USBDEVICE_H_ */
