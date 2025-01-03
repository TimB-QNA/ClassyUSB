#include "usbDev.h"
#include "packets/usbSetupPacket.h"
#include <string.h>

usbDev::usbDev(){
  uint8_t i;

  ud_vid=0x0000;
  ud_pid=0x0000;
  ud_rel=0x0000;
  ud_control.parent=this;
  ud_maxPower=1;
  ud_powerCfg=usbPowerConfig::BusPowered;
  
  // Default descriptor has no strings.
  ud_descriptor.iManufacturer = 0;
  ud_descriptor.iProduct      = 0;
  ud_descriptor.iSerialNumber = 0;

  // Language descriptor
  ud_languageList.bLength=4;
  ud_languageList.bDescriptorType=(uint8_t)usbDescriptorTypes::string;
  ud_languageList.langId[0] = 0x0809; // English (United Kingdom)

  nStringDesc=0;

  nComponents=0;
  for (i=0;i<USB_MAX_COMPONENTS;i++) ud_component[i]=nullptr;
  nInterfaces=0;

  for (i=0;i<USB_MAX_ENDPOINTS;i++) epList[i]=nullptr;
  nEndpoints=0;
  
  populateDescriptor();
}

void usbDev::initialise(){
  initialiseHardware();
  // Configure Endpoint 0 as control interface...
  addEndpoint(&ud_control);
}

void usbDev::setVendorID(uint16_t id){
  ud_vid=id;
}

void usbDev::setProductID(uint16_t id){
  ud_pid=id;
}

void usbDev::setProductVersion(uint8_t major, uint8_t minor, uint8_t fix){
  ud_rel=major<<8 | ((minor & 0x0F) << 4) | (fix & 0x0F);
}

void usbDev::setVendorString (const char *desc){
  ud_descriptor.iManufacturer = addStringDescriptor(desc);
}

void usbDev::setProductString(const char *desc){
  ud_descriptor.iProduct = addStringDescriptor(desc);
}

void usbDev::setSerialNumberString(const char *desc){
  ud_descriptor.iSerialNumber = addStringDescriptor(desc);
}

void usbDev::setConfigurationString(const char *desc){
  ud_descriptor.iSerialNumber = addStringDescriptor(desc);
}

bool usbDev::addComponent(usbComponent *component){
  if (nComponents==USB_MAX_COMPONENTS) return false;
  if (nEndpoints+component->endpoints()>=maxHardwareEndpoints) return false;

  ud_component[nComponents]=component;
  nInterfaces=ud_component[nComponents]->assignInterfaceNumbers(nInterfaces);

  if (component->m_componentName!=nullptr){
    component->usb_ifaceDesc.iInterface=addStringDescriptor(component->m_componentName);
  }

  if (component->m_subclass!=nullptr){
    if (component->m_subclass->m_componentName!=nullptr){
      component->m_subclass->usb_ifaceDesc.iInterface=addStringDescriptor(component->m_subclass->m_componentName);
    }
  }

  nComponents++;
  populateDescriptor();
  
  if (!addComponentEndpoints(component)) return false;

  if (component->m_subclass!=nullptr){
    if (!addComponentEndpoints(component->m_subclass)) return false;
  }

  return true;
}

bool usbDev::addComponentEndpoints(usbComponent *component){
  uint8_t i;

  for (i=0;i<component->endpoints();i++){
    if (!addEndpoint(componentEP(component, i))) return false;
  }

  return true;
}

bool usbDev::addEndpoint(usbEndpoint *ep){

  if (nEndpoints>=maxHardwareEndpoints) return false;
  ep->setHardware(this);
  ep->setHardwareEndpoint(nEndpoints);
  epList[nEndpoints]=ep;
  
  if (!addEndpointToHardware(nEndpoints, ep)) return false;

  nEndpoints++;
  return true;
}

usbEndpoint* usbDev::componentEP(usbComponent *component, uint8_t idx){
  return component->m_ep[idx];
}

uint8_t usbDev::componentEPs(usbComponent *component){
  return component->m_nEp;
}

void usbDev::exec(){
  uint8_t i;

  hardwareExec(); // Copy data from hardware to endpoints/components are required

  for (i=0;i<nComponents;i++) ud_component[i]->exec(); // exec components as appropriate.
}

void usbDev::populateDescriptor(){
  ud_descriptor.bLength            = 18;
  ud_descriptor.bDescriptorType    = (uint8_t)usbDescriptorTypes::device;
  ud_descriptor.bcdUSB             = 0x200; // USB 2.0
  ud_descriptor.bDeviceClass       = 0x00;  // Each interface specs own class
  ud_descriptor.bDeviceSubClass    = 0x00;  // See device class
  ud_descriptor.bDeviceProtocol    = 0x00;  // See device class
  ud_descriptor.bMaxPacketSize0    = 64;    // Must be 64 for high-speed
  ud_descriptor.idVendor           = ud_vid;       // Default only
  ud_descriptor.idProduct          = ud_pid;      // Default only
  ud_descriptor.bcdDevice          = ud_rel;  // Device release number (default 0)
  ud_descriptor.iManufacturer      = 0;       // No manufacturer string by default
  ud_descriptor.iProduct           = 0;            // No product string by default
  ud_descriptor.iSerialNumber      = 0;       // No serial number string by default
//  if (ud_stringDescriptors[0].hasString()) ud_descriptor.iManufacturer = 1;
//  if (ud_stringDescriptors[1].hasString()) ud_descriptor.iProduct      = 2;
//  if (ud_stringDescriptors[2].hasString()) ud_descriptor.iSerialNumber = 3;
  ud_descriptor.bNumConfigurations = 1;  // 1 possible configuration by default

  ud_qualifier.bLength             = 10;
  ud_qualifier.bDescriptorType     = (uint8_t)usbDescriptorTypes::deviceQualifier;
  ud_qualifier.bcdUSB              = 0x200; // USB2.0
  ud_qualifier.bDeviceClass        = 0x00;  // Each interface specs own class - see above
  ud_qualifier.bDeviceSubClass     = 0x00;
  ud_qualifier.bDeviceProtocol     = 0x00;
  ud_qualifier.bMaxPacketSize0     = 64; // We use 64 byte packets for all control ops.
  ud_qualifier.bNumConfigurations  = 0;  // No other speed configurations (worth a try).
  ud_qualifier.bReserved           = 0;
}

void usbDev::configDescriptor(uint8_t *buffer, uint16_t *len){
  uint8_t i;
  uint16_t offset;
  usbDev::configurationDescriptor cfg;
  
  offset=9; //Size of config descriptor header

  for (i=0;i<nComponents;i++){
    ud_component[i]->bufferInterfaceDescriptor(buffer, &offset);
  }

  cfg.bLength               = 9;
  cfg.bDescriptorType       = (uint8_t)usbDescriptorTypes::configuration;
  cfg.wTotalLength          = offset;  // offset is incremented per component, so we know the total number of bytes.
  cfg.bNumInterfaces        = nInterfaces;
  cfg.bConfigurationValue   = 1; //  Only ever one configuration at present, 1 just used as non-zero identifier.
  cfg.iConfiguration        = 0; //  Configuration string (none)
 // if (ud_stringDescriptors[3].hasString()) cfg.iConfiguration = 4;
  cfg.bmAttributes          = (uint8_t)ud_powerCfg | 0x80;
  cfg.bMaxPower             = ud_maxPower;

  memcpy(buffer, &cfg, 9);
  *len=offset;
}

void usbDev::setMaxPower(float current){
  ud_maxPower=(uint8_t)(current/2.+0.5);
}

uint8_t usbDev::addStringDescriptor(const char *desc){
  uint8_t i;
  for (i=0;i<USB_MAX_STRINGDESCRIPTORS;i++){
    if (!ud_stringDescriptors[i].hasString()){
      ud_stringDescriptors[i].setString(desc);
      return i+1;
    }
  }
  return 0;
}

usbDev::stringDescriptor::stringDescriptor(){
  bLength=2;
  nChars=0;
  bDescriptorType=(uint8_t)usbDescriptorTypes::string;
}

void usbDev::stringDescriptor::setString(const char *string){
  nChars=strlen(string);
  if (nChars>126) nChars=126; // Maximum representable string size (total length is a single byte so <=255)
  bLength=nChars*2+2;
  bString=(char*)string;
}

void usbDev::stringDescriptor::writeDescriptor(uint8_t *buffer, uint16_t *len){
  uint8_t i;
  uint16_t chr;

  memcpy(buffer+*len, &bLength, 1);         *len+=1;
  memcpy(buffer+*len, &bDescriptorType, 1); *len+=1;
  for (i=0;i<nChars;i++){
    chr=bString[i];
    memcpy(buffer+*len, &chr, 2);  *len+=2;
  }
}

bool usbDev::stringDescriptor::hasString(){
  return nChars>0;
}



usbDev::deviceEndpoint::deviceEndpoint() : usbEndpoint(64, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::in, usbEndpoint::endpointType::control)
{

}

void usbDev::deviceEndpoint::dataRecieved(uint16_t nBytes){
  // Handle Received data
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void usbDev::deviceEndpoint::setupRecieved(uint16_t nBytes){
  uint8_t tmp=0;
  uint8_t descBuffer[256];
  uint16_t descLen;

  uint8_t dbgTxn[64];
  memcpy(dbgTxn, (void*)outBuffer, 64);

  usbSetupPacket setup(outBuffer);
  
  writeInStall();

  switch (setup.request){
    case usbStandardRequestCode::setAddress:
      writeInZLP();
      parent->setAddress(setup.wValue);
      break;

    case usbStandardRequestCode::getDescriptor:
      if (setup.descriptor==usbDescriptorTypes::device){
        writeIn((uint8_t*)&parent->ud_descriptor, parent->ud_descriptor.bLength, setup.wLength);
      }
      if (setup.descriptor==usbDescriptorTypes::configuration){
        descLen=0;
        parent->configDescriptor(descBuffer, &descLen);
        writeIn(descBuffer, descLen, setup.wLength);
      }
      if (setup.descriptor==usbDescriptorTypes::deviceQualifier){
        writeIn((uint8_t*)&parent->ud_qualifier, parent->ud_qualifier.bLength, setup.wLength);
      }
      if (setup.descriptor==usbDescriptorTypes::string){
        descLen=0;
        if (setup.descriptorIndex==0){
          writeIn((uint8_t*)&parent->ud_languageList, parent->ud_languageList.bLength, setup.wLength);
        }else{
          parent->ud_stringDescriptors[setup.descriptorIndex-1].writeDescriptor(descBuffer, &descLen);
          writeIn(descBuffer, descLen, setup.wLength);
        }
      }
      break;

    case usbStandardRequestCode::setConfiguration:
      if (setup.wValue==1){
//        parent->sendData(this, &tmp, 0);
      }else{
//        parent->sendRequestError(this);
      }
      break;

    case usbStandardRequestCode::setFeature:
    // USB Specification 2.0 Page 259
      if (setup.bmRequestType==0){ // TEST_MODE feature is only one valid for USB Device
        if ((setup.wIndex >> 8) == 0x01) parent->setMode(usbModeTypes::TestModeJ);
        if ((setup.wIndex >> 8) == 0x02) parent->setMode(usbModeTypes::TestModeK);
        if ((setup.wIndex >> 8) == 0x03) parent->setMode(usbModeTypes::TestModeNAK);
        if ((setup.wIndex >> 8) == 0x04) parent->setMode(usbModeTypes::TestModePacket);
//        if ((setup.wIndex >> 8) == 0x05) setMode(usbModeTypes::TestModeJ);
      }
      break;
    case usbStandardRequestCode::clearFeature:
    // USB Specification 2.0 Page 259
    if (setup.bmRequestType==0){ // TEST_MODE feature is only one valid for USB Device
      if ((setup.wIndex >> 8) == 0x01) parent->setMode(usbModeTypes::TestModeJ);
      if ((setup.wIndex >> 8) == 0x02) parent->setMode(usbModeTypes::TestModeK);
      if ((setup.wIndex >> 8) == 0x03) parent->setMode(usbModeTypes::TestModeNAK);
      if ((setup.wIndex >> 8) == 0x04) parent->setMode(usbModeTypes::TestModePacket);
      //        if ((setup.wIndex >> 8) == 0x05) setMode(usbModeTypes::TestModeJ);
    }
    break;
  };
}
#pragma GCC pop_options
