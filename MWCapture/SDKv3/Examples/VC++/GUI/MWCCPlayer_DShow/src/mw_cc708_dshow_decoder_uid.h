#ifndef MWCEA708DECODERUID_H
#define MWCEA708DECODERUID_H

#include <initguid.h>

// {7690CA55-2542-4BB5-A133-5A2FABBEA36A}
DEFINE_GUID(CLSID_MWCC708Decoder, 0x7690ca55, 0x2542, 0x4bb5, 0xa1, 0x33, 0x5a,
	    0x2f, 0xab, 0xbe, 0xa3, 0x6a);

// {BF837162-B42B-4C4D-818E-2DE668276845}
DEFINE_GUID(CLSID_MWCC708DecoderProp, 0xbf837162, 0xb42b, 0x4c4d, 0x81, 0x8e,
	    0x2d, 0xe6, 0x68, 0x27, 0x68, 0x45);

// Media Types
#ifndef OUR_GUID_ENTRY
#define OUR_GUID_ENTRY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
#endif

#ifndef KSDATAFORMAT_TYPE_SDI_CC708
OUR_GUID_ENTRY(KSDATAFORMAT_TYPE_SDI_CC708, 0x4a45b931, 0xff5c, 0x42aa, 0xa5,
	       0xa2, 0x43, 0x77, 0x1b, 0x93, 0xeb, 0xef);
#endif

#ifndef KSDATAFORMAT_SUBTYPE_SDI_CC708_Packet
OUR_GUID_ENTRY(KSDATAFORMAT_SUBTYPE_SDI_CC708_Packet, 0x4a45b932, 0xff5c,
	       0x42aa, 0xa5, 0xa2, 0x43, 0x77, 0x1b, 0x93, 0xeb, 0xef);
#endif

#endif
