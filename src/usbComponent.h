#ifndef USBCOMPONENT_H_
#define USBCOMPONENT_H_

#ifndef USB_MAX_COMPONENT_ENDPOINTS
  #define USB_MAX_COMPONENT_ENDPOINTS 3
#endif

#include <stdint.h>
#include "usbEndpoint.h"
class usbDev;

class usbComponent{
  friend class usbDev;
//  friend uint8_t usbComponent::subclassCode(); // allow access to subclassCode
//  friend uint8_t usbComponent::protocolCode(); // allow access to control protocol code

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

  public:
    usbComponent(uint8_t classCode=0, uint8_t classControlProtocol=0, usbComponent *subclass=nullptr, const char *name=nullptr);
	void initialise(usbDev *usbHardware);
    uint8_t endpoints();
    virtual void exec();
    
    interfaceDescriptor usb_ifaceDesc;
    virtual void bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len);
    virtual uint8_t subclassCode();
    virtual uint8_t protocolCode();

  protected:
    const char* m_componentName;
    uint8_t m_cmpNameIdx;
    usbComponent *m_subclass;

    bool assignEndpoint(usbEndpoint *ep);
    uint8_t       m_nEp=0;
    usbEndpoint  *m_ep[USB_MAX_COMPONENT_ENDPOINTS];

    uint8_t assignInterfaceNumbers(uint8_t start);

    virtual void updateInterfaceDescriptor();
    void bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len);
};

#include "usbDev.h"

#endif
