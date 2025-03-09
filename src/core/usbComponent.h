#ifndef USBCOMPONENT_H_
#define USBCOMPONENT_H_

#ifndef USB_MAX_COMPONENT_INTERFACES
  #define USB_MAX_COMPONENT_INTERFACES 3
#endif

#include <stdint.h>
#include "usbInterface.h"
#include "packets/usbSetupPacket.h"
class usbDev;

class usbComponent{
  friend class usbDev;

  public:
    enum class classCodes : uint8_t { none = 0x00, CDC=0x02, HID=0x03 };

    usbComponent(const char *name=nullptr);
    void initialise(usbDev *usbHardware);
    uint8_t endpoints();
    virtual void exec(uint64_t millis);

  protected:
    const char* m_componentName;
    
    bool addInterface(usbInterface *iface);
    uint8_t        m_nIface=0;
    usbInterface  *m_iface[USB_MAX_COMPONENT_INTERFACES];

    uint8_t assignInterfaceNumbers(uint8_t start);

    virtual void initComponent();
    void bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len);
    
    bool usbClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
    bool usbInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt);

    virtual bool handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
    virtual bool handleInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
};

#include "usbDev.h"
#endif
