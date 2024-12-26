/*
 * usbDev.h
 *
 * Created: 13/08/2024 18:05:34
 *  Author: Tim
 */ 

#ifndef USBDEV_H_
#define USBDEV_H_

#include <stdint.h>
#include "usbEndpoint.h"
#include "usbComponent.h"
#include "usbTypes.h"

#ifndef USB_MAX_COMPONENTS
  #define USB_MAX_COMPONENTS 3
#endif

#ifndef USB_MAX_ENDPOINTS
 #define USB_MAX_ENDPOINTS 8
#endif

#ifndef USB_MAX_STRINGDESCRIPTORS
  #define USB_MAX_STRINGDESCRIPTORS 8
#endif


// Define an array of available endpoints based on hardware in your hardware implementation. The idea is to make it as easy as possible to handle interrupts.
class usbDev
{
  friend class usbEndpoint;

  private:
    class deviceEndpoint : public usbEndpoint{
      public:
        enum class dataTransferDirection   : uint8_t { toDevice=0, toHost=1 };
        enum class requestType             : uint8_t { standard=0, classObj=1, vendor=2, reserved=3 };
        enum class recipientType           : uint8_t { device=0,   interface=1, endpoint=2, other=3 };
        enum class standardFeatureSelector : uint8_t { deviceRemoteWakeup=1, endpointHalt=0, testMode=2 };

        deviceEndpoint();
        void dataRecieved(uint16_t nBytes);
        void setupRecieved(uint16_t nBytes);
        usbDev *parent;
    };

    friend class deviceEndpoint;

    typedef struct {
      uint8_t  bLength;
      uint8_t  bDescriptorType;
      uint16_t bcdUSB;
      uint8_t  bDeviceClass;
      uint8_t  bDeviceSubClass;
      uint8_t  bDeviceProtocol;
      uint8_t  bMaxPacketSize0;
      uint16_t idVendor;
      uint16_t idProduct;
      uint16_t bcdDevice;
      uint8_t  iManufacturer;
      uint8_t  iProduct;
      uint8_t  iSerialNumber;
      uint8_t  bNumConfigurations;
    } deviceDescriptor;

    typedef struct {
      uint8_t  bLength;
      uint8_t  bDescriptorType;
      uint16_t bcdUSB;
      uint8_t  bDeviceClass;
      uint8_t  bDeviceSubClass;
      uint8_t  bDeviceProtocol;
      uint8_t  bMaxPacketSize0;
      uint8_t  bNumConfigurations;
      uint8_t  bReserved;
    } deviceQualifier;

    typedef struct {
      uint8_t  bLength;
      uint8_t  bDescriptorType;
      uint16_t wTotalLength;
      uint8_t  bNumInterfaces;
      uint8_t  bConfigurationValue;
      uint8_t  iConfiguration;
      uint8_t  bmAttributes;
      uint8_t  bMaxPower;
    }configurationDescriptor;

    typedef struct {
      uint8_t  bLength;
      uint8_t  bDescriptorType;
      uint16_t langId[1];
    }languageDescriptor;

    class stringDescriptor{
      public:
        stringDescriptor();
        void setString(const char *string);
        void writeDescriptor(uint8_t *buffer, uint16_t *len);
        bool hasString();

      private:
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t nChars;
        char  *bString;
    };

  public:
    usbDev();
    
    void initialise();

    void setVendorID(uint16_t id);
    void setProductID(uint16_t id);
    void setProductVersion(uint8_t major, uint8_t minor, uint8_t fix);

    void setVendorString(const char *desc);
    void setProductString(const char *desc);
    void setSerialNumberString(const char *desc);
    void setConfigurationString(const char *desc);

    void setMaxPower(float current);

    bool addComponent(usbComponent *component);

    void exec();

  protected:
    uint8_t     nEndpoints;
    uint8_t     maxHardwareEndpoints;
    usbEndpoint *epList[USB_MAX_ENDPOINTS];

    uint16_t ud_vid;
    uint16_t ud_pid;
    uint16_t ud_rel;
    uint16_t ud_defaultLang;

    uint8_t        ud_maxPower;
    usbPowerConfig ud_powerCfg;

    const char *ud_vendorDesc;
    const char *ud_productDesc;
    usbDev::deviceEndpoint     ud_control;
    usbDev::deviceDescriptor   ud_descriptor;
    usbDev::deviceQualifier    ud_qualifier;
    usbDev::languageDescriptor ud_languageList;
    usbDev::stringDescriptor   ud_stringDescriptors[USB_MAX_STRINGDESCRIPTORS];
    uint8_t nStringDesc;

    uint8_t nInterfaces;
    uint8_t nComponents;
    usbComponent *ud_component[USB_MAX_COMPONENTS];
   
    bool addComponentEndpoints(usbComponent *component);
    bool addEndpoint(usbEndpoint *ep);

    usbEndpoint* componentEP(usbComponent *component, uint8_t idx);
    uint8_t componentEPs(usbComponent *component);

    void    populateDescriptor();
    void    configDescriptor(uint8_t *buffer, uint16_t *len);
    uint8_t addStringDescriptor(const char *desc);
     
  public:
    virtual void attach()=0;

  protected:
    // Hardware Configuration
    virtual void initialiseHardware()=0;
    virtual void setAddress(uint8_t addr)=0;
    virtual void setMode(usbModeTypes mode)=0;
    virtual bool addEndpointToHardware(uint8_t idx, usbEndpoint *ep)=0;

    // Hardware Comms Routines
    virtual uint16_t writeIn(uint8_t ep, uint16_t nBytes)=0;
    virtual void     writeInZLP(uint8_t ep)=0;
    virtual void     writeInStall(uint8_t ep)=0;

    // Hardware operation
    virtual void hardwareExec()=0;
};

#endif /* USBDEV_H_ */