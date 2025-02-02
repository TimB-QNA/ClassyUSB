#ifndef USBCOMPONENT_H_
#define USBCOMPONENT_H_

#ifndef USB_MAX_COMPONENT_ENDPOINTS
  #define USB_MAX_COMPONENT_ENDPOINTS 3
#endif

#include <stdint.h>
#include "usbEndpoint.h"
#include "packets/usbSetupPacket.h"
class usbDev;
class usbSubComponent;

class usbComponent{
  friend class usbDev;
  friend class usbSubComponent;

  public:
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

  typedef struct{
    uint8_t  bFunctionLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t bcdCDC;
  }headerDescriptor;
	
  public:
    usbComponent(usbSubComponent *subclass=nullptr, uint8_t classCode=0x00, uint8_t subclassCode=0x00, uint8_t protocolCode=0x00, const char *name=nullptr);
	void initialise(usbDev *usbHardware);
    uint8_t endpoints();
    virtual void exec();
    
    virtual uint8_t subclassCode();
    virtual uint8_t protocolCode();

  protected:
    const char* m_componentName;
    uint8_t m_cmpNameIdx;
    usbSubComponent *m_subclass=nullptr;
	
    usbComponent::interfaceDescriptor usb_ifaceDesc;
	
    bool assignEndpoint(usbEndpoint *ep);
    uint8_t       m_nEp=0;
    usbEndpoint  *m_ep[USB_MAX_COMPONENT_ENDPOINTS];

    uint8_t assignInterfaceNumbers(uint8_t start);

    virtual void initComponent()=0;
    void bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len);
	virtual void bufferSupplementalInterfaceDescriptor(uint8_t *buffer, uint16_t *len);
    virtual void bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len);
	
	void usbClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
	virtual void handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
};

#include "usbDev.h"
#include "usbSubComponent.h"
#endif
