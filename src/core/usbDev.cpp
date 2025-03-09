#include "usbDev.h"
#include "packets/usbSetupPacket.h"
#include <string.h>

usbDev::usbDev(){
  uint8_t i;

  ud_vid=0x0000;
  ud_pid=0x0000;
  ud_rel=0x0000;
  ud_maxPower=1;
  ud_powerCfg=usbPowerConfig::BusPowered;
  
  // Default descriptor has no strings.
  m_iConfiguration=0;

  // Language descriptor
  ud_languageList.bLength=4;
  ud_languageList.bDescriptorType=(uint8_t)usbDescriptorTypes::string;
  ud_languageList.langId[0] = 0x0809; // English (United Kingdom)

  nComponents=0;
  for (i=0;i<USB_MAX_COMPONENTS;i++) ud_component[i]=nullptr;
  nInterfaces=0;

  nEndpoints=0;
  ud_control = new usbDev::deviceEndpoint(this);
   
  populateDescriptor();
}

void usbDev::initialise(){  
  initialiseHardware();
  // Configure Endpoint 0 as control interface...
  ud_control->initialise(this);
  addEndpoint(ud_control);
}

void usbDev::setVendorID(uint16_t id){
  ud_descriptor.idVendor = id;
}

void usbDev::setProductID(uint16_t id){
  ud_descriptor.idProduct = id;
}

void usbDev::setProductVersion(uint8_t major, uint8_t minor, uint8_t fix){
  ud_descriptor.bcdDevice = major<<8 | ((minor & 0x0F) << 4) | (fix & 0x0F);
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
  m_iConfiguration = addStringDescriptor(desc);
}

bool usbDev::addComponent(usbComponent *component){
  if (nComponents==USB_MAX_COMPONENTS) return false;
  if (nEndpoints+component->endpoints()>=maxHardwareEndpoints) return false;

  ud_component[nComponents]=component;
  nInterfaces=ud_component[nComponents]->assignInterfaceNumbers(nInterfaces);

  nComponents++;
  
  if (!addComponentEndpoints(component)) return false;
  
  component->initialise(this);

  return true;
}

bool usbDev::addComponentEndpoints(usbComponent *component){
  uint8_t i, j;

  for (i=0;i<component->m_nIface;i++){
    for (j=0;j<component->m_iface[i]->m_nEp;j++){
      if (!addEndpoint(component->m_iface[i]->m_ep[j])) return false;
    }
  }

  return true;
}

bool usbDev::addEndpoint(usbEndpoint *ep){

  if (nEndpoints>=maxHardwareEndpoints) return false;
  
  if (!addEndpointToHardware(nEndpoints, ep)) return false;

  nEndpoints++;
  return true;
}

void usbDev::exec(uint64_t millis){
  uint8_t i;

  hardwareExec(millis); // Copy data from hardware to endpoints/components are required

  for (i=0;i<nComponents;i++) ud_component[i]->exec(millis); // exec components as appropriate.
}

void usbDev::populateDescriptor(){
  ud_descriptor.bLength            = 18;
  ud_descriptor.bDescriptorType    = (uint8_t)usbDescriptorTypes::device;
  ud_descriptor.bcdUSB             = 0x200; // USB 2.0
  ud_descriptor.bDeviceClass       = 0x00;  // Each interface specs own class
  ud_descriptor.bDeviceSubClass    = 0x00;  // See device class
  ud_descriptor.bDeviceProtocol    = 0x00;  // See device class
  ud_descriptor.bMaxPacketSize0    = 64;    // Must be 64 for high-speed
  ud_descriptor.idVendor           = 0;     // Default only
  ud_descriptor.idProduct          = 0;     // Default only
  ud_descriptor.bcdDevice          = 0;     // Device release number (default 0)
  ud_descriptor.iManufacturer      = 0;     // No manufacturer string by default
  ud_descriptor.iProduct           = 0;     // No product string by default
  ud_descriptor.iSerialNumber      = 0;     // No serial number string by default
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
  usbDev::configurationDescriptor cfg;
  
  *len=9;
  
  // Write the interface desciptors first starting at byte 9
  // because ClassyUSB is dynamic and therefore we don't know
  // the total descriptor length in advance. The total length
  // is used below, which is actually the first stanza of the
  // descriptor.
  for (i=0;i<nComponents;i++){
    ud_component[i]->bufferInterfaceDescriptor(buffer, len);
  }
  
  cfg.bLength               = 9;
  cfg.bDescriptorType       = (uint8_t)usbDescriptorTypes::configuration;
  cfg.wTotalLength          = *len;  // len is incremented per component, so we know the total number of bytes.
  cfg.bNumInterfaces        = nInterfaces;
  cfg.bConfigurationValue   = 1; //  Only ever one configuration at present, 1 just used as non-zero identifier.
  cfg.iConfiguration        = m_iConfiguration; //  Configuration string (none as default)
  cfg.bmAttributes          = (uint8_t)ud_powerCfg | 0x80;
  cfg.bMaxPower             = ud_maxPower;

  memcpy(buffer, &cfg, 9);
  
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



usbDev::deviceEndpoint::deviceEndpoint(usbDev* usbDevice) : usbEndpoint(512, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::both, usbEndpoint::endpointType::control)
{
  m_device=usbDevice;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void usbDev::deviceEndpoint::dataRecieved(uint16_t nBytes){
  uint8_t tmp=0;
  uint16_t descLen=0;

  usbSetupPacket setup(outBuffer);

  if (setup.bmRequestType.type==usbSetupPacket::rqType::usbClass){
    for (tmp=0; tmp<m_device->nComponents; tmp++){
        m_device->ud_component[tmp]->usbClassRequest(this, setup);
    }
    writeStall();
    return;
  }

  if (setup.bmRequestType.recipient==usbSetupPacket::rqRecipient::interface){
    for (tmp=0; tmp<m_device->nComponents; tmp++){
      m_device->ud_component[tmp]->usbInterfaceRequest(this, setup);
    }
    writeStall();
    return;
  }
  
  if (setup.bmRequestType.data==0x00){
    if (setup.bRequest.request==usbStandardRequestCode::setAddress){
      writeZLP();
      m_device->setAddress(setup.wValue);
      return;
    }
    
    if (setup.bRequest.request==usbStandardRequestCode::setConfiguration){
      if (setup.wValue==1){ writeZLP(); return; }
    }
  }
  
  switch (setup.bRequest.request){
    case usbStandardRequestCode::getDescriptor:
      if (setup.descriptor==usbDescriptorTypes::device){
        write((uint8_t*)&m_device->ud_descriptor, m_device->ud_descriptor.bLength, setup.wLength);
      }
      if (setup.descriptor==usbDescriptorTypes::configuration){
        descLen=0;
        m_device->configDescriptor(inBuffer, &descLen);
        write(inBuffer, descLen, setup.wLength);
      }
      if (setup.descriptor==usbDescriptorTypes::deviceQualifier){
        write((uint8_t*)&m_device->ud_qualifier, m_device->ud_qualifier.bLength, setup.wLength);
      }
      if (setup.descriptor==usbDescriptorTypes::string){
        descLen=0;
        if (setup.descriptorIndex==0){
          write((uint8_t*)&m_device->ud_languageList, m_device->ud_languageList.bLength, setup.wLength);
        }else{
          m_device->ud_stringDescriptors[setup.descriptorIndex-1].writeDescriptor(inBuffer, &descLen);
          write(inBuffer, descLen, setup.wLength);
        }
      }
      break;



    case usbStandardRequestCode::setFeature:
    // USB Specification 2.0 Page 259
      if (setup.bmRequestType.data==0){ // TEST_MODE feature is only one valid for USB Device
        if ((setup.wIndex >> 8) == 0x01) m_device->setMode(usbModeTypes::TestModeJ);
        if ((setup.wIndex >> 8) == 0x02) m_device->setMode(usbModeTypes::TestModeK);
        if ((setup.wIndex >> 8) == 0x03) m_device->setMode(usbModeTypes::TestModeNAK);
        if ((setup.wIndex >> 8) == 0x04) m_device->setMode(usbModeTypes::TestModePacket);
//        if ((setup.wIndex >> 8) == 0x05) setMode(usbModeTypes::TestModeJ);
      }
      break;
    case usbStandardRequestCode::clearFeature:
    // USB Specification 2.0 Page 259
    if (setup.bmRequestType.data==0){ // TEST_MODE feature is only one valid for USB Device
      if ((setup.wIndex >> 8) == 0x01) m_device->setMode(usbModeTypes::TestModeJ);
      if ((setup.wIndex >> 8) == 0x02) m_device->setMode(usbModeTypes::TestModeK);
      if ((setup.wIndex >> 8) == 0x03) m_device->setMode(usbModeTypes::TestModeNAK);
      if ((setup.wIndex >> 8) == 0x04) m_device->setMode(usbModeTypes::TestModePacket);
      //        if ((setup.wIndex >> 8) == 0x05) setMode(usbModeTypes::TestModeJ);
    }
    break;
  };
  
  writeStall();
}
#pragma GCC pop_options
