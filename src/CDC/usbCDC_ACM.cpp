#include "usbCDC_ACM.h"
#include "usbCDC.h"
#include <string.h>

usbCDC_ACM::usbCDC_ACM(uint8_t *txBuffSpace, uint16_t txSize, uint8_t *rxBuffSpace, uint16_t rxSize) : usbCDC("CDC_ACM")
{
  ctrlEp = new usbCDC_ACM::cdcAcmEndpoint(this, usbCDC_ACM::cdcAcmEndpoint::opMode::alert,    64, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::in,  usbEndpoint::endpointType::interrupt);
  dInEp  = new usbCDC_ACM::cdcAcmEndpoint(this, usbCDC_ACM::cdcAcmEndpoint::opMode::transmit, 64, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::in,  usbEndpoint::endpointType::bulk);
  dOutEp = new usbCDC_ACM::cdcAcmEndpoint(this, usbCDC_ACM::cdcAcmEndpoint::opMode::receive,  64, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::out, usbEndpoint::endpointType::bulk);   
    
  m_mainIface.addEndpoint(ctrlEp);
  m_dataIface.addEndpoint(dInEp);
  m_dataIface.addEndpoint(dOutEp);
  m_mainIface.dataInterface=&m_dataIface;

  addInterface(&m_mainIface);
  addInterface(&m_dataIface);

  rxBuffer.init(rxBuffSpace, rxSize);
  txBuffer.init(txBuffSpace, txSize);
}

void usbCDC_ACM::initComponent(){

}

uint16_t usbCDC_ACM::write(uint8_t *data, uint16_t nBytes){
  uint16_t i;
  uint16_t nb=0;

  for (i=0;i<nBytes;i++) txBuffer.enqueue(data[i]);
  if (!dInEp->isWriting()){
    nb=txBuffer.dequeueBlock(dInEp->inBuffer, dInEp->bufferSize);
    dInEp->write(nullptr, nb);
  }
  return nb;
}

usbCDC_ACM::cdcAcmEndpoint::cdcAcmEndpoint(usbCDC_ACM *p, usbCDC_ACM::cdcAcmEndpoint::opMode mode, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType) : usbEndpoint(bSize, sz,uDir,uType){
  parent=p;
  m_mode=mode;
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void usbCDC_ACM::cdcAcmEndpoint::dataRecieved(uint16_t nBytes){
  uint16_t i;

  if (m_mode==usbCDC_ACM::cdcAcmEndpoint::opMode::receive){
    for (i=0;i<nBytes;i++) parent->rxBuffer.enqueue(outBuffer[i]);
  }
}
#pragma GCC pop_options

void usbCDC_ACM::cdcAcmEndpoint::transmitComplete(){
  uint16_t nb=0;

  if (m_mode==usbCDC_ACM::cdcAcmEndpoint::opMode::transmit){
    // Write data from tx buffer to endpoint if applicable
//    nb=parent->txBuffer.dequeueBlock(inBuffer, bufferSize);
//    write(nullptr, nb);
  }
}

usbCDC_ACM::cdcMainInterface::cdcMainInterface() : usbInterface((uint8_t)usbComponent::classCodes::CDC, (uint8_t)usbCDC::subclassCodes::abstractControlModel, 0, 0, "CDC Main"){
//  m_dataInterface=dataInterface;
}

void usbCDC_ACM::cdcMainInterface::preEndpointDescriptor(uint8_t *buffer, uint16_t *len){
  usbCDC::headerDescriptor hdrDesc;
  usbCDC::capabilityDescriptor capability;
  usbCDC::functionalDescriptor fncDesc;

  hdrDesc.bFunctionLength=0x05;
  hdrDesc.bDescriptorType=(uint8_t)usbCDC::descriptorTypes::cs_interface;
  hdrDesc.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::headerFunctionalDescriptor;
  hdrDesc.bcdCDC=0x1001;
  memcpy(buffer+*len, &hdrDesc, hdrDesc.bFunctionLength); *len+=hdrDesc.bFunctionLength;

  // Supported functionality
  capability.bFunctionLength=0x04;
  capability.bDescriptorType=(uint8_t)usbCDC::descriptorTypes::cs_interface;
  capability.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::acmFunctional;
  capability.bmCapabilities=0x02; //0x0F; // Supports line coding - See CDC PSTN Subclass Docs Page 12
  memcpy(buffer+*len, &capability, capability.bFunctionLength); *len+=capability.bFunctionLength;

  // Interface setup
  fncDesc.bFunctionLength=0x05;
  fncDesc.bDescriptorType=(uint8_t)usbCDC::descriptorTypes::cs_interface;
  fncDesc.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::unionFunctional;
  fncDesc.bFnData[0]=interfaceNumber();                  // Control Interface - This interface
  fncDesc.bFnData[1]=dataInterface->interfaceNumber();   // Subordinate Interface - Data
  memcpy(buffer+*len, &fncDesc, fncDesc.bFunctionLength); *len+=fncDesc.bFunctionLength;

  // Control management descriptor
  fncDesc.bFunctionLength=0x05;
  fncDesc.bDescriptorType=(uint8_t)usbCDC::descriptorTypes::cs_interface;
  fncDesc.bDescriptorSubtype=(uint8_t)usbCDC::descriptorSubTypes::callManagement;
  fncDesc.bFnData[0]=0x00;                                // Internal call management, with encapsulated command - See CDC PSTN Subclass Docs Page 12
  fncDesc.bFnData[1]=dataInterface->interfaceNumber();    // Data interface if multiplexing commands
  memcpy(buffer+*len, &fncDesc, fncDesc.bFunctionLength); *len+=fncDesc.bFunctionLength;

  /* We don't have notification at the moment.

  // Notification Endpoint descriptor
  0x07, // 0 bLength
  USB_DESCTYPE_ENDPOINT, // 1 bDescriptorType
  (CDC_NOTIFICATION_EP_NUM | 0x80), // 2 bEndpointAddress
  0x03, // 3 bmAttributes
  0x40, // 4 wMaxPacketSize - Low
  0x00, // 5 wMaxPacketSize - High
  0xFF, // 6 bInterval

  */
}

usbCDC_ACM::cdcDataInterface::cdcDataInterface() : usbInterface(10, 0, 0, 0, "CDC Data"){
}