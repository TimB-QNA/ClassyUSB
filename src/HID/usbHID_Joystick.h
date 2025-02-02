#ifndef USBHID_JOYSTICK_H_
#define USBHID_JOYSTICK_H_

#include <stdint.h>
#include "../core/usbComponent.h"

class usbHID_Joystick : public usbComponent
{
  public:
    usbHID_Joystick(uint8_t axes, uint8_t buttons);

  protected:
    usbEndpoint *inEndpoint;

    void initComponent();

    void bufferSupplementalInterfaceDescriptor(uint8_t *buffer, uint16_t *len);
	
	void handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
	
	enum class tagDataType : uint8_t { main=0x00, global=0x01, local=0x02};
	enum class tagFunction : uint8_t { input=0x08, output=0x09, feature=0xB, collection=0x0A,
		                                                               endCollection=0x0ClongTag=0x0F};	
};

#endif /* USBCDC_H_ */