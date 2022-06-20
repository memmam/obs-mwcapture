#ifndef HEVC_NALU_H
#define HEVC_NALU_H
#include <stdio.h>

bool HevcFindSps(unsigned char *pucStream, int uiLen, int *piSpsStart, int *piSpsEnd);
#endif