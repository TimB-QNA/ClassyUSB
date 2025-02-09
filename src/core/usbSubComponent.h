#ifndef __USBSUBCOMPONENT_H__
#define __USBSUBCOMPONENT_H__

#include "usbEndpoint.h"
#include "usbComponent.h"

class usbSubComponent
{
  friend class usbComponent;
  friend class usbDev;
  
  public:
    usbSubComponent(uint8_t subclassCode=0x00, uint8_t protocolCode=0x00);
	
	uint8_t endpoints();
	
  protected:
    usbComponent *m_parent=nullptr;
	uint8_t m_subclassCode=0;
	uint8_t m_protocolCode=0;
	
	usbComponent::interfaceDescriptor sub_ifaceDesc;
	
	uint8_t       m_nEp=0;
	usbEndpoint  *m_ep[USB_MAX_COMPONENT_ENDPOINTS];
	
	void initialise(usbComponent *parent);
	bool assignEndpoint(usbEndpoint *ep);
	
    virtual void initComponent()=0;
	
	void bufferInterfaceDescriptor(uint8_t *buffer, uint16_t *len);
	uint8_t assignInterfaceNumbers(uint8_t start);
	
	virtual void bufferFunctionalDescriptor(uint8_t *buffer, uint16_t *len)=0;
	
	void usbClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
	virtual void handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
};


#endif //__USBSUBCOMPONENT_H__