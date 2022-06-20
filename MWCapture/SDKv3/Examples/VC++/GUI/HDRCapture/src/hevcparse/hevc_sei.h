#ifndef HEVC_SEI_H
#define HEVC_SEI_H
#include <stdio.h>
typedef struct _HDMI_HDR_INFOFRAME {
	unsigned char   ucEOTF;
	unsigned char   ucMetadataDescriptorID;

	unsigned short  usDisplayPromariesX[3];
	unsigned short  usDisplayPromariesY[3];

	unsigned short  usWhitePointX;
	unsigned short  usWhitePointY;

	unsigned int    uiMaxDisplayMasteringLuminance;
	unsigned int    uiMinDisplayMasteringLuminance;

	unsigned short  usMaxContentLightLevel;
	unsigned short  usMaxFrameAverageLightLevel;
} HDMI_HDR_INFOFRAME;

unsigned char* CreateHdrSeiPrefix(HDMI_HDR_INFOFRAME* pstHdrInfo, unsigned int* puiLen);

#endif

