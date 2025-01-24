/*
 * usbStandardTypes.h
 *
 * Created: 02/10/2024 22:17:33
 *  Author: Tim
 */ 
 
#ifndef USBSTANDARDTYPES_H_
#define USBSTANDARDTYPES_H_

#include <stdint.h>

enum class usbStandardRequestCode     : uint8_t { getStatus=0,        clearFeature=1,     setFeature=3,    setAddress=5,    getDescriptor=6, setDescriptor=7,
                                                  getConfiguration=8, setConfiguration=9, getInterface=10, setInterface=11, synchFrame=12 };

enum class usbDescriptorTypes         : uint16_t { invalid=0, device=1, configuration=2, string=3, interface=4, endpoint=5, deviceQualifier=6, 
                                                   otherSpeedConfiguration=7, interfacePower=8, hubDescriptor=0x29,  // USB 2 Standard page 251, Table 9-5
                                                   cs_interface=0x24, cs_endpoint=0x25 }; // CDC120 doc page 17

enum class usbModeTypes               : uint16_t { None=0, LowSpeed, HighSpeed, FullSpeed, TestModeHighSpeed, TestModeNAK, TestModeJ, TestModeK, TestModePacket, Device, Host };

enum class usbPowerConfig             : uint8_t  { BusPowered=0x80, SelfPowered=0x40, RemoteWakeup=0x20 };

#endif /* USBSTANDARDTYPES_H_ */