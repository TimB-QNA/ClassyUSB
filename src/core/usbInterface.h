#ifndef __USBINTERFACE_H__
#define __USBINTERFACE_H__

#ifndef USB_MAX_INTERFACE_ENDPOINTS
  #define USB_MAX_INTERFACE_ENDPOINTS 3
#endif

#include <stdint.h>
#include "usbEndpoint.h"
#include "packets/usbSetupPacket.h"

class usbInterface
{
  friend class usbComponent;
  friend class usbDev;

  public:
    #pragma pack(push, 1)
    typedef struct{
      uint8_t  bLength;             // Size of descriptor (always 9)
      uint8_t  bDescriptorType;     // Interface descriptor Type  - USB Standard page 251
      uint8_t  bInterfaceNumber;    // Number of this interface, 0-based array for multiple interfaces -- This is set by usbDev when the component is added.
      uint8_t  bAlternateSetting;   // Only used with alternate settings????
      uint8_t  bNumEndpoints;       // Number of endpoints used by component
      uint8_t  bInterfaceClass;     // Component class
      uint8_t  bInterfaceSubClass;  // Component subclass code
      uint8_t  bInterfaceProtocol;  // Component-specific protocol
      uint8_t  iInterface;          // String descriptor Index
    }interfaceDescriptor;
    #pragma pack(pop)

    usbInterface(uint8_t interfaceClass, uint8_t interfaceSubClass, uint8_t interfaceProtocol, uint8_t alternateSetting=0, const char *iFaceName=nullptr);
    bool addEndpoint(usbEndpoint *ep);
    void initialise(usbDev *usbHardware);
    void bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len);
    bool usbInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt);

    uint8_t interfaceNumber();                   // Used by other interfaces as a reference
    uint8_t endpoints();                         // Used in usbComponent for hardware setup

  protected:
    const char*   m_ifaceName;
    uint8_t       m_nEp=0;
    usbEndpoint  *m_ep[USB_MAX_INTERFACE_ENDPOINTS];   // Accessed in usbDev for hardware setup
    usbInterface::interfaceDescriptor stdDescriptor;

    virtual void initInterface();
    virtual void preEndpointDescriptor(uint8_t *buffer, uint16_t *len);
    virtual void postEndpointDescriptor(uint8_t *buffer, uint16_t *len);

    virtual bool handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
    virtual bool handleInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt);

  private:
    void    assignInterfaceNumber(uint8_t idx);  // Used in usbComponent for hardware setup
};


#endif //__USBINTERFACE_H__