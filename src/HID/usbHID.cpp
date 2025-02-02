#include "usbHID.h"
#include <string.h>

usbHID::inEndpoint::inEndpoint() : usbEndpoint(64, usbEndpoint::endpointSize::b64, usbEndpoint::endpointDirection::in, usbEndpoint::endpointType::interrupt){
}
