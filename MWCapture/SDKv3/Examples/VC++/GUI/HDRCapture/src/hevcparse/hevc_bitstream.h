#ifndef HEVC_BITSTREAM_H
#define HEVC_BITSTREAM_H
#include <stdio.h>

typedef struct st_HEVC_BITSTREAMER{
    unsigned char * pucData;//data
    unsigned char* pucByte;//point to now pucData pucByte= pucByte + uiIdx;
    bool bCheckZero;
    int uiLen;//datelen
    int uiIdx;//current pos
    int uiBits;//left bit <= 8
    //int uiZeros;//not use
    int uiWrited;//have write bits
}ST_HEVC_BITSTREAMER;

void HevcBsInitWrite(ST_HEVC_BITSTREAMER *pstBs, unsigned char *pucStreamBuf, unsigned int uiLen);
void HevcWriteWord(ST_HEVC_BITSTREAMER *pstBs, int bits, unsigned int value);
void HevcWriteUE(ST_HEVC_BITSTREAMER *pstBs, unsigned int value);
int HevcGetBsBits(ST_HEVC_BITSTREAMER *pstBs);

void HevcBsInitRead(ST_HEVC_BITSTREAMER *pstBs, unsigned char *pucStreamBuf, unsigned int uiLen, bool bCheckZero);
unsigned int HevcGetBits(ST_HEVC_BITSTREAMER *pstBs, int iNum);
unsigned int HevcGetGolombU(ST_HEVC_BITSTREAMER *pstBs);
int HevcGetGolombS(ST_HEVC_BITSTREAMER *pstBs);

int HevcReplace(unsigned char* pucSrcData, unsigned int uiSrclen, unsigned char* pucDstData, unsigned int *pucDstlen);
#endif
