#include "usbCDC.h"
#include <string.h>

usbCDC::usbCDC(const char* name) : usbComponent(name){
}

usbCDC::cdcEndpoint::cdcEndpoint(usbCDC *p, uint16_t bSize, usbEndpoint::endpointSize sz, usbEndpoint::endpointDirection uDir, usbEndpoint::endpointType uType) : usbEndpoint(bSize, sz, uDir, uType){
  parent=p;
}

void usbCDC::cdcEndpoint::dataRecieved(uint16_t nBytes){
  // Handle Received data
}

