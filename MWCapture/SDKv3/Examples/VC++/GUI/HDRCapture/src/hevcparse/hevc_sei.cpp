#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hevc_sei.h"
#include "hevc_bitstream.h"
unsigned char* CreateHdrSeiPrefix(HDMI_HDR_INFOFRAME* pstHdrInfo, unsigned int* puiLen)
{
	unsigned char aucSeiTemp[34];
    unsigned char* pucSeiStream;
    ST_HEVC_BITSTREAMER stBs;
    if ((NULL == pstHdrInfo) || (NULL == puiLen)){
        printf("CreateHdrSeiPrefix in parm\n");
        return NULL;
    }
    pucSeiStream = (unsigned char*)malloc(34 + 20 + 6);
    if (NULL == pucSeiStream){
        printf("CreateHdrSeiPrefix malloc 60\n");
        return NULL;
    }
    memset(aucSeiTemp, 0, 34);
    memset(pucSeiStream, 0, 60);

    HevcBsInitWrite(&stBs, aucSeiTemp, 34);
    HevcWriteWord(&stBs, 8, 0x90);
    HevcWriteWord(&stBs, 8, 0x04);

    HevcWriteWord(&stBs, 16, pstHdrInfo->usMaxContentLightLevel);
    HevcWriteWord(&stBs, 16, pstHdrInfo->usMaxFrameAverageLightLevel);

    HevcWriteWord(&stBs, 8, 0x89);
    HevcWriteWord(&stBs, 8, 0x18);

	for (int i = 0; i < 3; i++)
	{
        HevcWriteWord(&stBs, 16, pstHdrInfo->usDisplayPromariesX[i]);
        HevcWriteWord(&stBs, 16, pstHdrInfo->usDisplayPromariesY[i]);
	}

    HevcWriteWord(&stBs, 16, pstHdrInfo->usWhitePointX);
    HevcWriteWord(&stBs, 16, pstHdrInfo->usWhitePointY);

    HevcWriteWord(&stBs, 32, pstHdrInfo->uiMaxDisplayMasteringLuminance * 10000);
    HevcWriteWord(&stBs, 32, pstHdrInfo->uiMinDisplayMasteringLuminance);

    HevcWriteWord(&stBs, 8, 0x00);
    HevcWriteWord(&stBs, 8, 0x80);
    pucSeiStream[3] = 0x01;
    pucSeiStream[4] = 0x4e;
    pucSeiStream[5] = 0x01;
    *puiLen = 60 - 6;
    if (HevcReplace(aucSeiTemp, 34, pucSeiStream + 6, puiLen) < 0){
        free(pucSeiStream);
        printf("HevcReplace fail\n");
        return NULL;
    }
    *puiLen += 6;
    return pucSeiStream;

}

