#ifndef USBHID_JOYSTICK_H_
#define USBHID_JOYSTICK_H_

#ifndef MAX_USB_JOYSTICK_CHANS
  #define MAX_USB_JOYSTICK_CHANS 8
#endif

#include <stdint.h>
#include "../core/usbComponent.h"
#include "usbHID.h"

class usbHID_Joystick : public usbComponent
{
  public:
    enum class channelDataType    : uint8_t { boolean=0x00, u8, u16, u32, i8, i16, i32};
    enum class channelLogicalType : uint8_t { button=0x00, selector, linear};
    enum class channelUsage       : uint8_t { undefined=0x00, flightSimDevice, spaceSimDevice, airplaneSimDevice, heliSimDevice,
                                              aileron, aileronTrim, antiTorqueCtrl, autopilotEnable, chaffRelease, collectiveCtrl, cyclicCtrl, cyclicTrim, diveBrake, electronicCountermeasures, elevator, elevatorTrim, flightComms, flareRelease};
  public:
    class channel{
      friend class usbHID_Joystick;
      public:
        channel(usbHID_Joystick::channelLogicalType logicalType, usbHID_Joystick::channelDataType dataType, int32_t logicalMin, int32_t logicalMax, usbHID_Joystick::channelUsage usage);
        void setValue(int32_t val);
        
      protected:
        usbHID_Joystick::channelLogicalType m_logicalType;
        usbHID_Joystick::channelDataType m_dataType;
        usbHID_Joystick::channelUsage m_usage;
        int32_t m_logicalMin, m_logicalMax;
        int32_t m_value;
        usbHID_Joystick *parent;
    };
    
    class hidJoystickInterface : public usbInterface
    {
      public:
        hidJoystickInterface();
        usbHID::config *hidConfig;

      protected:
        void preEndpointDescriptor(uint8_t *buffer, uint16_t *len);
        void postEndpointDescriptor(uint8_t *buffer, uint16_t *len);
    };

    usbHID_Joystick();
    bool addChannel(channel *userChan);
    void exec(uint64_t millis);

  protected:
    hidJoystickInterface iface;
    usbEndpoint  *inEndpoint;
    usbHID::config hidConfig;
    
    bool handleClassRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
    bool handleInterfaceRequest(usbEndpoint *replyEp, usbSetupPacket pkt);
        
    void update();
        
    uint16_t m_nChans;
    channel* chan[MAX_USB_JOYSTICK_CHANS];
    
    enum class tagDataType : uint8_t { main=0x00, global=0x01, local=0x02};
    enum class tagFunction : uint8_t { input=0x08, output=0x09, feature=0xB, collection=0x0A, endCollection=0x0, longTag=0x0F};    
};

#endif /* USBCDC_H_ */