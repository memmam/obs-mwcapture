#include "hevc_bitstream.h"
#include <math.h>
#define CHAR_BITS 8
void HevcBsInitWrite(ST_HEVC_BITSTREAMER *pstBs, unsigned char *pucStreamBuf, unsigned int uiLen)
{
    pstBs->pucData = pucStreamBuf;
    pstBs->uiLen = uiLen;
    pstBs->uiIdx = 0;
    pstBs->uiBits = 0;
    pstBs->pucByte = NULL;
    //pstBs->uiZeros = 0;
    pstBs->uiWrited = 0;
}
unsigned char* GetBYTE(ST_HEVC_BITSTREAMER *pstBs)
{
    if (pstBs->uiIdx >= pstBs->uiLen)
        return 0;
    return &(pstBs->pucData[pstBs->uiIdx++]);
}

void WriteBit(ST_HEVC_BITSTREAMER *pstBs, unsigned char bit)
{
    pstBs->uiWrited++;
    if (pstBs->uiBits == 0){
        pstBs->pucByte = GetBYTE(pstBs);
        pstBs->uiBits = CHAR_BITS;
    }
    pstBs->uiBits--;
    (*pstBs->pucByte) |= bit << pstBs->uiBits;
}


void HevcWriteWord(ST_HEVC_BITSTREAMER *pstBs, int bits, unsigned int value)
{
    unsigned char u = 0;
    while (bits > 0){
        bits--;
        u = value >> bits & 0x01;
        WriteBit(pstBs, u);
    }
}
void HevcWriteUE(ST_HEVC_BITSTREAMER *pstBs, unsigned int value)
{
    int i;
    int m = static_cast<int>(log((long double)(value + 1)) / log((long double)2)); // 前缀，也即是组号
    int info = static_cast<int>(value + 1 - pow(2.0, (int)m)); // 组内的偏移量
    for (i = 0; i < m; i++){
        HevcWriteWord(pstBs, 1, 0);
    }

    HevcWriteWord(pstBs, 1, 1);
    HevcWriteWord(pstBs, m, info);
}

int HevcGetBsBits(ST_HEVC_BITSTREAMER *pstBs)
{
    return pstBs->uiWrited;
}

void HevcBsInitRead(ST_HEVC_BITSTREAMER *pstBs, unsigned char *pucStreamBuf, unsigned int uiLen, bool bCheckZero)
{
    pstBs->pucData = pucStreamBuf;
    pstBs->uiLen = uiLen;
    pstBs->uiIdx = 0;
    pstBs->uiBits = CHAR_BITS - 1;
    pstBs->bCheckZero = bCheckZero;
}

int HevcGetBit(ST_HEVC_BITSTREAMER *pstBs)
{
    int res;
    if (pstBs->uiIdx >= pstBs->uiLen){
        return false;
    }
    res = (int)(pstBs->pucData[pstBs->uiIdx] & 1 << pstBs->uiBits);
    pstBs->uiBits--;
    if (pstBs->uiBits < 0){
        pstBs->uiBits = CHAR_BITS - 1;
        pstBs->uiIdx++;

        if (pstBs->uiIdx >= 2 && pstBs->bCheckZero == true){
            if (pstBs->pucData[pstBs->uiIdx - 2] == 0 && pstBs->pucData[pstBs->uiIdx - 1] == 0 && pstBs->pucData[pstBs->uiIdx] == 3){
                pstBs->uiIdx++;
            }
        }
    }
    return res;
}


unsigned int HevcGetBits(ST_HEVC_BITSTREAMER *pstBs, int iNum)
{
    int i;
    //assert(iNum <= 32);
    unsigned int result = 0;
    for (i = 0; i < iNum; i++)
    if (HevcGetBit(pstBs)){
        result |= 1 << (iNum - i - 1);
    }
    return result;
}


void HevcSkipBits(ST_HEVC_BITSTREAMER *pstBs, int iNum)
{
    unsigned int scipBytes = iNum / CHAR_BITS;
    if (pstBs->uiIdx >= 2){
        if (pstBs->pucData[pstBs->uiIdx - 2] == 0 && pstBs->pucData[pstBs->uiIdx - 1] == 0 && pstBs->pucData[pstBs->uiIdx] == 3){
            pstBs->uiIdx++;
        }
    }

    while (scipBytes){
        scipBytes--;
        pstBs->uiIdx++;
        if (pstBs->uiIdx >= 2){
            if (pstBs->pucData[pstBs->uiIdx - 2] == 0 && pstBs->pucData[pstBs->uiIdx - 1] == 0 && pstBs->pucData[pstBs->uiIdx] == 3){
                pstBs->uiIdx++;
            }
        }
    }

    if (pstBs->uiBits > iNum % CHAR_BITS){
        pstBs->uiBits -= iNum % CHAR_BITS;
    }
    else{
        pstBs->uiIdx++;
        pstBs->uiBits = pstBs->uiBits - iNum % CHAR_BITS + CHAR_BITS;
    }
}

unsigned int HevcShowBits(ST_HEVC_BITSTREAMER *pstBs, int iNum)
{
    int i;
    //assert(iNum <= 32);
    
    unsigned int uiPrevIdx = pstBs->uiIdx;
    unsigned int uiPrevBits = pstBs->uiBits;
    unsigned int result = 0;
    for (i = 0; i < iNum; i++){
        if (HevcGetBit(pstBs)){
            result |= 1 << (iNum - i - 1);        
        }
    }
    pstBs->uiIdx = uiPrevIdx;
    pstBs->uiBits = uiPrevBits;

    return result;
}



unsigned int HevcGetGolombU(ST_HEVC_BITSTREAMER *pstBs)
{
    int iZeroBits = 0;
    while (!HevcGetBit(pstBs)){
        iZeroBits++;
    }

    if (iZeroBits >= 32){
        return 0;
    }
    //    throw std::range_error("Golomb: size of value more then 32 bits");
    return (1 << iZeroBits) - 1 + HevcGetBits(pstBs, iZeroBits);
}



int HevcGetGolombS(ST_HEVC_BITSTREAMER *pstBs)
{
    int buf = HevcGetGolombU(pstBs);

    if (buf & 1)
        buf = (buf + 1) >> 1;
    else
        buf = -(buf >> 1);

    return buf;
}

int HevcReplace(unsigned char* pucSrcData, unsigned int uiSrclen, unsigned char* pucDstData, unsigned int *pucDstlen)
{
    unsigned int i;
    unsigned int uiDstLen = 0;
    unsigned int uiCanAdd = *pucDstlen - uiSrclen;

    for (i = 0; i < uiSrclen - 1;){
        if (pucSrcData[i]){
            pucDstData[uiDstLen++] = pucSrcData[i++];
            continue;
        }
        if (pucSrcData[i + 1]){
            pucDstData[uiDstLen++] = pucSrcData[i++];
            pucDstData[uiDstLen++] = pucSrcData[i++];
            continue;
        }

        if (uiCanAdd <= 0){
            return -1;
        }
        uiCanAdd--;
        pucDstData[uiDstLen++] = 0;
        pucDstData[uiDstLen++] = 0;
        pucDstData[uiDstLen++] = 0x3;
        i += 2;
    }
    if (i < uiSrclen){
        pucDstData[uiDstLen++] = pucSrcData[i++];
    }
    *pucDstlen = uiDstLen;
    return 0;
}