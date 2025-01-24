#ifndef USBTYPECONVERSION_H_
#define USBTYPECONVERSION_H_

// Set platform endian to 0 for little endian and 1 for big endian
// Ensure byte-swapping macros exist for all system endiannesses
// If PLATFORM_ENDIAN isn't set, it's assumed to be little endian.

#ifndef PLATFORM_ENDIAN
  #define PLATFORM_ENDIAN 0
#endif

#define swap_u16(u16) ((uint16_t)(((uint16_t)(u16) >> 8) | ((uint16_t)(u16) << 8)))

#if PLATFORM_ENDIAN==0
  #define fromLE_u16(u16) u16

  #define fromBE_u16(u16) swap_u16(u16)
#endif

#if PLATFORM_ENDIAN==1
  #define fromLE_u16(u16) swap_u16(u16)

  #define fromBE_u16(u16) u16
#endif


#endif /* TYPECONVERSION_H_ */