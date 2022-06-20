#include "hevc_nalu.h"
enum en_NAL_UNIT_TYPE {
	EN_NAL_UNIT_TYPE_TRAIL_N = 0,
	EN_NAL_UNIT_TYPE_TRAIL_R = 1,
	EN_NAL_UNIT_TYPE_TSA_N = 2,
	EN_NAL_UNIT_TYPE_TSA_R = 3,
	EN_NAL_UNIT_TYPE_STSA_N = 4,
	EN_NAL_UNIT_TYPE_STSA_R = 5,
	EN_NAL_UNIT_TYPE_RADL_N = 6,
	EN_NAL_UNIT_TYPE_RADL_R = 7,
	EN_NAL_UNIT_TYPE_RASL_N = 8,
	EN_NAL_UNIT_TYPE_RASL_R = 9,
	EN_NAL_UNIT_TYPE_BLA_W_LP = 16,
	EN_NAL_UNIT_TYPE_BLA_W_RADL = 17,
	EN_NAL_UNIT_TYPE_BLA_N_LP = 18,
	EN_NAL_UNIT_TYPE_IDR_W_RADL = 19,
	EN_NAL_UNIT_TYPE_IDR_N_LP = 20,
	EN_NAL_UNIT_TYPE_CRA_NUT = 21,
	EN_NAL_UNIT_TYPE_IRAP_VCL23 = 23,
	EN_NAL_UNIT_TYPE_VPS = 32,
	EN_NAL_UNIT_TYPE_SPS = 33,
	EN_NAL_UNIT_TYPE_PPS = 34,
	EN_NAL_UNIT_TYPE_AUD = 35,
	EN_NAL_UNIT_TYPE_EOS_NUT = 36,
	EN_NAL_UNIT_TYPE_EOB_NUT = 37,
	EN_NAL_UNIT_TYPE_FD_NUT = 38,
	EN_NAL_UNIT_TYPE_SEI_PREFIX = 39,
	EN_NAL_UNIT_TYPE_SEI_SUFFIX = 40,
} EN_NAL_UNIT_TYPE;

bool HevcFindSps(unsigned char *pucStream, int uiLen, int *piSpsStart,
		 int *piSpsEnd)
{
	unsigned char *pucData = pucStream + 2;
	unsigned char *pucEnd = pucStream + uiLen - 4;
	if ((piSpsStart == NULL) || (piSpsEnd == NULL) || (pucStream == 0)) {
		printf("in parm err\n");
		return false;
	}
	*piSpsStart = -1;
	*piSpsEnd = -1;
	if (uiLen > 2048) {
		pucEnd = pucStream + 2048 - 4;
	}
	while (pucData < pucEnd) {
		if (*pucData > 1) {
			pucData += 3;
			continue;
		}
		if (*pucData == 0) {
			pucData++;
			continue;
		}
		if ((pucData[-1] != 0) || (pucData[-2] != 0)) {
			pucData += 3;
			continue;
		}
		if (EN_NAL_UNIT_TYPE_SPS == ((*(pucData + 1) & 0x7e) >> 1)) {
			*piSpsStart = pucData - pucStream - 2;
			pucData += 4;
			continue;
		}
		if (-1 == *piSpsStart) {
			pucData += 4;
			continue;
		}

		do {
			pucData--;
		} while (0 == *pucData);
		*piSpsEnd = pucData - pucStream + 1;
		break;
	}
	if (-1 == *piSpsEnd) {
		return false;
	}
	return true;
}
#if 0
#include <string.h>
void main()
{
    int iSpsStart, iSpsEnd;
    unsigned char aucOutBuf[512];
    unsigned char aucTestBuf[4096];
    FILE *fp = fopen("test.h265","rb");
    fread(aucTestBuf, 1, 4096, fp);
    HevcFindSps(aucTestBuf, 4096, &iSpsStart, &iSpsEnd);
    memcpy(aucOutBuf, aucTestBuf + iSpsStart, iSpsEnd - iSpsStart);
}
#endif