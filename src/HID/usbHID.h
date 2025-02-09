#ifndef USBHID_H_
#define USBHID_H_

#include <stdint.h>
#include "../core/usbEndpoint.h"

class usbHID
{
  public:
    enum class subclassCodes : uint8_t { none = 0x00, bootInterface = 0x01 };
    enum class protocolCodes : uint8_t { none = 0x00, keyboard = 0x01, mouse = 0x02 };
    enum class countryCodes  : uint8_t { notSupported =  0,
		                                       arabic =  1,
											  belgian =  2,
				                   canadian_bilingual =  3,
									  canadian_french =  4,
									    czechRepublic =  5,
										       danish =  6,
											  finnish =  7,
										 	   french =  8,
											   german =  9,
											    greek = 10,
											   hebrew = 11,
											  hungary = 12,
									international_ISO = 13,
											  italian = 14,
											    japan = 15,
											   korean = 16,
									    latinAmerican = 17,
										  netherlands = 18,
									  	    norwegian = 19,
											  persian = 20,
											   poland = 21,
										    portugese = 22,
											   russia = 23,
											 slovakia = 24,
											  spanish = 25,
											  swedish = 26,
									     swiss_french = 27,
						   			     swiss_german = 28,
										  switzerland = 29,
											   taiwan = 30,
											  turkish = 31,
											       uk = 32,
											       us = 33,
										   yugoslavia = 34,
											turkish_f = 35 };

    enum class descriptorCode   : uint8_t { HID = 0x21, report = 0x22, physicalDescriptor = 0x23 };
	enum class classRequestCode : uint8_t { getReport=0x01, getIdle=0x02, getProtocol=0x03, setReport=0x09, setIdle=0x0A, setProtocol=0x0B };
	
	#pragma pack(push, 1)
    typedef struct {
	  uint8_t  bLength;
	  uint8_t  bDescriptorType;
	  uint16_t bcdHID;
	  uint8_t  bCountryCode;
	  uint8_t  bNumDescriptors;
	  uint8_t  bFollowingDescriptorType;
	  uint16_t bFollowingDescriptorLength;
    }hidDescriptor;
	#pragma pack(pop)
	
    class inEndpoint : public usbEndpoint {
	  public:
	    inEndpoint();
	};
};

#endif /* USBCDC_H_ */