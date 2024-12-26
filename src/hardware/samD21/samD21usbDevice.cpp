#include "samd21usbDevice.h"
#include "sam.h"
#include <string.h>

typedef union{
  UsbDeviceDescBank    bank[2];
  struct
  {
    UsbDeviceDescBank  out;
    UsbDeviceDescBank  in;
  };
} hwEpDesc;

static samd21usbDevice *_samd21usb_usbHw;
static hwEpDesc hwEp[SAMUSB21_MAX_EPS] alignas(4);

extern "C" {
  void USB_Handler(){
    _samd21usb_usbHw->interruptHandler(); 
  }
};

void samd21usbDevice::interruptHandler(){
  // USB Interrupt handler
  uint8_t i;

/*
  if (USB->DEVICE.INTFLAG.bit.LPMSUSP=1) ; // Link power management suspend
  if (USB->DEVICE.INTFLAG.bit.LPMNYET=1) ; // Link power management not yet
  if (USB->DEVICE.INTFLAG.bit.RAMACER=1) ; // Ram access interrupt flag
  if (USB->DEVICE.INTFLAG.bit.UPRSM=1) ;   // Upstream resume flag
  if (USB->DEVICE.INTFLAG.bit.EORSM=1) ;   // End of resume flag
  if (USB->DEVICE.INTFLAG.bit.WAKEUP=1) ;  // Wakeup
*/
  if (USB->DEVICE.INTFLAG.bit.EORST==1){
    // End of reset
    USB->DEVICE.INTFLAG.bit.EORST=1;
    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.RXSTP=1;  // Interrupt on setup packet
    for (i=1; i<nEndpoints; i++) configureEndpoint(i);
  }
/*
  if (USB->DEVICE.INTFLAG.bit.SOF=1) ;     // Start of frame
  if (USB->DEVICE.INTFLAG.bit.MSOF=1) ;    // Micro start of frame
  if (USB->DEVICE.INTFLAG.bit.SUSPEND=1) ; // Suspend
*/

  for (i=0;i<nEndpoints;i++){
    if (USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.bit.RXSTP==1){
//      USB->DEVICE.DeviceEndpoint[i].EPSTATUSSET.bit.STALLRQ1=1;
      epList[i]->setupRecieved(hwEp[i].out.PCKSIZE.bit.BYTE_COUNT); // OUT (from host) transaction
//      readComplete(i, 0);
    }
    USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg=0xFF;
  }

  USB->DEVICE.INTFLAG.reg=0xFF;
}

samd21usbDevice::samd21usbDevice() : usbDev() {
  maxHardwareEndpoints=8;  // Maximum number of endpoints supported by this hardware - used in usbDev.
  _samd21usb_usbHw=this;
}

uint8_t samd21usbDevice::epSizeCode(usbEndpoint::endpointSize sz){
  switch(sz){
    case usbEndpoint::endpointSize::b8:
      return 0x00;
    case usbEndpoint::endpointSize::b16:
      return 0x01;
    case usbEndpoint::endpointSize::b32:
      return 0x02;
    case usbEndpoint::endpointSize::b64:
      return 0x03;
    case usbEndpoint::endpointSize::b128:
      return 0x04;
    case usbEndpoint::endpointSize::b256:
      return 0x05;
    case usbEndpoint::endpointSize::b512:
      return 0x06;
    case usbEndpoint::endpointSize::b1023:
      return 0x07;
  };
  return 0x00;
}

bool samd21usbDevice::addEndpointToHardware(uint8_t idx, usbEndpoint *ep){
  hwEp[idx].out.PCKSIZE.bit.SIZE=epSizeCode(ep->size);
  hwEp[idx].out.ADDR.reg=(uint32_t)ep->outBuffer; // reinterpret_cast<uint32_t>(ep->outBuffer);

  hwEp[idx].in.PCKSIZE.bit.SIZE=epSizeCode(ep->size);
  hwEp[idx].in.ADDR.reg=(uint32_t)ep->inBuffer; // reinterpret_cast<uint32_t>(ep->inBuffer);
  configureEndpoint(idx);

  return true;
}

void samd21usbDevice::configureEndpoint(uint8_t idx){
  uint8_t epType;

  epType=0;
  if (epList[idx]->type==usbEndpoint::endpointType::control)    epType = 0x01;
  if (epList[idx]->type==usbEndpoint::endpointType::isochronos) epType = 0x02;
  if (epList[idx]->type==usbEndpoint::endpointType::bulk)       epType = 0x03;
  if (epList[idx]->type==usbEndpoint::endpointType::interrupt)  epType = 0x04;
  if (epList[idx]->type==usbEndpoint::endpointType::dual)       epType = 0x05;
    
  USB->DEVICE.DeviceEndpoint[idx].EPCFG.bit.EPTYPE0=epType;
  USB->DEVICE.DeviceEndpoint[idx].EPCFG.bit.EPTYPE1=epType;
  USB->DEVICE.DeviceEndpoint[idx].EPINTENSET.bit.RXSTP=1;  // Interrupt on setup packet
  USB->DEVICE.DeviceEndpoint[idx].EPINTENSET.bit.TRCPT0=1; // Interrupt on receipt
  USB->DEVICE.DeviceEndpoint[idx].EPSTATUSCLR.reg=0xFF; // Clear register
}


void samd21usbDevice::initialiseHardware(){
  configurePins();
  configureClock();

  // Config USB module
  USB->DEVICE.CTRLA.bit.SWRST = 1; // Software Reset USB
  while (USB->DEVICE.SYNCBUSY.bit.SWRST); /* Wait for reset */

  // Clear endpoint data...
  memset((void*)hwEp, 0, sizeof(hwEp));
  USB->DEVICE.DESCADD.reg=(uint32_t)hwEp;
   
  USB->DEVICE.CTRLA.bit.MODE   = 0; // USB Device
  USB->DEVICE.CTRLA.bit.RUNSTDBY = 1; // Enable Run in Standby
  
  setMode(usbModeTypes::FullSpeed);
  USB->DEVICE.CTRLB.bit.DETACH=0;

  USB->DEVICE.INTENSET.bit.EORST=1;

  USB->DEVICE.CTRLA.bit.ENABLE = 1; // Enable USB
  while (USB->DEVICE.SYNCBUSY.bit.ENABLE);

  NVIC_EnableIRQ(USB_IRQn);
  //  NVIC_SetPriority(USB_IRQn, 4); // One lower than RTC
}

/*!
 * This function configures the clocks required for USB. It expects GCLK0
 * to output a 48MHz clock.
 */
void samd21usbDevice::configureClock(){
  PM->APBBMASK.bit.USB_=1; // Enable APB Clock for USB
//  PM->AHBMASK.bit.USB_=1;  // Enable AHB Clock for USB

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(USB_GCLK_ID);
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
}

void samd21usbDevice::configurePins(){
  PORT->Group[0].DIR.reg = (PORT->Group[0].DIR.reg |    (1<<24) | (1<<25) ); // Set PA24 and PA25 to output
  PORT->Group[0].OUT.reg = (PORT->Group[0].OUT.reg & ~( (1<<24) | (1<<25) )); // Set PA24 and PA25 low
  PORT->Group[0].PINCFG[24].reg=0; // Disable any multiplexing, pullup etc.
  PORT->Group[0].PINCFG[25].reg=0; // Disable any multiplexing, pullup etc.

  PORT->Group[0].PMUX[12].reg=0x66; // Set USB peripheral for PA24 and PA25.

  PORT->Group[0].PINCFG[24].reg=1; // Enable Multiplex
  PORT->Group[0].PINCFG[25].reg=1; // Enable Multiplex

  #define NVM_USB_PAD_TRANSN_POS 45
  #define NVM_USB_PAD_TRANSN_SIZE 5
  #define NVM_USB_PAD_TRANSP_POS 50
  #define NVM_USB_PAD_TRANSP_SIZE 5
  #define NVM_USB_PAD_TRIM_POS 55
  #define NVM_USB_PAD_TRIM_SIZE 3

  uint32_t pad_transn
           = (*((uint32_t *)(NVMCTRL_OTP4) + (NVM_USB_PAD_TRANSN_POS / 32)) >> (NVM_USB_PAD_TRANSN_POS % 32))
	     & ((1 << NVM_USB_PAD_TRANSN_SIZE) - 1);
  uint32_t pad_transp
	    = (*((uint32_t *)(NVMCTRL_OTP4) + (NVM_USB_PAD_TRANSP_POS / 32)) >> (NVM_USB_PAD_TRANSP_POS % 32))
	      & ((1 << NVM_USB_PAD_TRANSP_SIZE) - 1);
  uint32_t pad_trim = (*((uint32_t *)(NVMCTRL_OTP4) + (NVM_USB_PAD_TRIM_POS / 32)) >> (NVM_USB_PAD_TRIM_POS % 32))
	              & ((1 << NVM_USB_PAD_TRIM_SIZE) - 1);

  if (pad_transn == 0x1F) pad_transn = 5;
  if (pad_transp == 0x1F) pad_transp = 29;
  if (pad_trim == 0x7) pad_trim = 5;

  USB->DEVICE.PADCAL.reg = USB_PADCAL_TRANSN(pad_transn) | USB_PADCAL_TRANSP(pad_transp) | USB_PADCAL_TRIM(pad_trim);
  USB->DEVICE.QOSCTRL.bit.CQOS = 3;
  USB->DEVICE.QOSCTRL.bit.DQOS = 3;
}

void samd21usbDevice::attach(){
  USB->DEVICE.CTRLB.bit.DETACH=0; // Attach to USB BUS
}

void samd21usbDevice::hardwareExec(){
//  if (USB->DEVICE.CTRLB.bit.DETACH==1) USB->DEVICE.CTRLB.bit.DETACH=0; // Attach to USB BUS
  interruptHandler();
}

// Set device address on bus.
void samd21usbDevice::setAddress(uint8_t addr){
  USB->DEVICE.DADD.bit.ADDEN = 0x00;
  USB->DEVICE.DADD.bit.DADD  = addr & 0x7F;
  USB->DEVICE.DADD.bit.ADDEN = 1;
}

void samd21usbDevice::setMode(usbModeTypes mode){
  switch(mode){
    case usbModeTypes::None:
      break;
    case usbModeTypes::LowSpeed:
      USB->DEVICE.CTRLB.bit.SPDCONF=0x01;
      break;
    case usbModeTypes::HighSpeed:
      USB->DEVICE.CTRLB.bit.SPDCONF=0x02;
      break;
    case usbModeTypes::FullSpeed:
      USB->DEVICE.CTRLB.bit.SPDCONF=0x00;
      break;
    case usbModeTypes::TestModeHighSpeed:
      USB->DEVICE.CTRLB.bit.OPMODE2=1;
      USB->DEVICE.CTRLB.bit.SPDCONF=0x03;
      break;
    case usbModeTypes::TestModeNAK:
      USB->DEVICE.CTRLB.bit.OPMODE2=1;
      USB->DEVICE.CTRLB.bit.GNAK=1;
      break;
    case usbModeTypes::TestModeJ:
      USB->DEVICE.CTRLB.bit.OPMODE2=1;
      USB->DEVICE.CTRLB.bit.TSTJ=1;
      break;
    case usbModeTypes::TestModeK:
      USB->DEVICE.CTRLB.bit.OPMODE2=1;
      USB->DEVICE.CTRLB.bit.TSTK=1;
      break;
    case usbModeTypes::TestModePacket:
      USB->DEVICE.CTRLB.bit.OPMODE2=1;
      USB->DEVICE.CTRLB.bit.TSTPCKT=1;
      break;

    case usbModeTypes::Host:
    case usbModeTypes::Device:
      // Ignored so the compiler doesn't produce warning (these modes are not relevant here)
      break;
  }
}

void samd21usbDevice::readComplete(uint8_t dEp, uint8_t bnk){
  hwEp[dEp].bank[bnk].PCKSIZE.bit.BYTE_COUNT=0;
  hwEp[dEp].bank[bnk].PCKSIZE.bit.MULTI_PACKET_SIZE=0;
  if (bnk==0){
    USB->DEVICE.DeviceEndpoint[dEp].EPSTATUSCLR.bit.BK0RDY=1;
    USB->DEVICE.DeviceEndpoint[dEp].EPINTFLAG.bit.TRCPT0=1;
  }else{
    USB->DEVICE.DeviceEndpoint[dEp].EPSTATUSCLR.bit.BK1RDY=1;
    USB->DEVICE.DeviceEndpoint[dEp].EPINTFLAG.bit.TRCPT1=1;
  }
  USB->DEVICE.DeviceEndpoint[dEp].EPINTFLAG.bit.RXSTP=1;
}

uint16_t samd21usbDevice::writeIn(uint8_t ep, uint16_t nBytes){
  hwEp[ep].in.PCKSIZE.bit.BYTE_COUNT=nBytes;
  hwEp[ep].in.PCKSIZE.bit.MULTI_PACKET_SIZE=0;

  USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.TRCPT1=1;  // Clear Transfer complete flag
  USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.TRFAIL1=1;  // Clear Transfer failure flag
//  USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.STALLRQ1=1;
  USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK1RDY=1;

  /* Wait for transfer to complete */
  while (USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.TRCPT1==0 && USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.TRFAIL1==0)
  {
  }
  return nBytes;
}

void samd21usbDevice::writeInZLP(uint8_t ep){
  writeIn(ep, 0);
}

void samd21usbDevice::writeInStall(uint8_t ep){
  USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ1=1;
}