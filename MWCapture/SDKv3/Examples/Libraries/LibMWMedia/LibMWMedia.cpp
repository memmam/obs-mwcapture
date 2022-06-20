// LibMWMedia.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "MWDXRender.h"
#include "DSoundRenderer.h"
#include "LibMWMedia\MWMedia.h"

bool LIBMWMEDIA_API MWGetD3D9RenderSupportFormat(int *pnFmtNum, DWORD *pdwFmt)
{
	if (pdwFmt == NULL) {
		if (pnFmtNum == NULL)
			return false;
		else {
			*pnFmtNum = 37;
			return true;
		}
	}

	if (pnFmtNum == NULL)
		return false;

	if (*pnFmtNum < 37)
		return false;
	else {
		*pnFmtNum = 37;
		if (IsBadWritePtr(pdwFmt, sizeof(DWORD) * 37))
			return false;
		else {
			*pnFmtNum = 37;
			pdwFmt[0] = MWFOURCC_GREY;
			pdwFmt[1] = MWFOURCC_Y800;
			pdwFmt[2] = MWFOURCC_Y8;
			pdwFmt[3] = MWFOURCC_Y16;
			pdwFmt[4] = MWFOURCC_RGB16;
			pdwFmt[5] = MWFOURCC_BGR16;
			pdwFmt[6] = MWFOURCC_YUY2;
			pdwFmt[7] = MWFOURCC_YUYV;
			pdwFmt[8] = MWFOURCC_BGRA;
			pdwFmt[9] = MWFOURCC_RGBA;
			pdwFmt[10] = MWFOURCC_ARGB;
			pdwFmt[11] = MWFOURCC_ABGR;
			pdwFmt[12] = MWFOURCC_BGR24;
			pdwFmt[13] = MWFOURCC_RGB24;
			pdwFmt[14] = MWFOURCC_UYVY;
			pdwFmt[15] = MWFOURCC_NV12;
			pdwFmt[16] = MWFOURCC_NV21;
			pdwFmt[17] = MWFOURCC_NV16;
			pdwFmt[18] = MWFOURCC_NV61;
			pdwFmt[19] = MWFOURCC_YVYU;
			pdwFmt[20] = MWFOURCC_VYUY;
			pdwFmt[21] = MWFOURCC_I422;
			pdwFmt[22] = MWFOURCC_YV16;
			pdwFmt[23] = MWFOURCC_I420;
			pdwFmt[24] = MWFOURCC_IYUV;
			pdwFmt[25] = MWFOURCC_YV12;
			pdwFmt[26] = MWFOURCC_P010;
			pdwFmt[27] = MWFOURCC_P210;
			pdwFmt[28] = MWFOURCC_IYU2;
			pdwFmt[29] = MWFOURCC_V308;
			pdwFmt[30] = MWFOURCC_AYUV;
			pdwFmt[31] = MWFOURCC_UYVA;
			pdwFmt[32] = MWFOURCC_V408;
			pdwFmt[33] = MWFOURCC_VYUA;
			pdwFmt[34] = MWFOURCC_Y410;
			pdwFmt[35] = MWFOURCC_RGB10;
			pdwFmt[36] = MWFOURCC_BGR10;

			return true;
		}
	}
}

HD3DRENDERER
LIBMWMEDIA_API
MWCreateD3DRenderer(int cx, int cy, DWORD dwFourcc, bool bReverse, HWND hWnd)
{
	MWDXRender *pRet = new MWDXRender();
	if (!pRet->initialize(cx, cy, dwFourcc, bReverse, hWnd, MW_CSP_BT_709,
			      MW_CSP_LEVELS_TV, MW_CSP_LEVELS_PC)) {
		delete pRet;
		pRet = NULL;
	}
	return (HD3DRENDERER)pRet;
}

void LIBMWMEDIA_API MWDestroyD3DRenderer(HD3DRENDERER hRenderer)
{
	if (hRenderer == NULL)
		return;

	MWDXRender *pRet = (MWDXRender *)hRenderer;
	pRet->cleanup_device();
	delete pRet;
}

BOOL LIBMWMEDIA_API MWD3DRendererPushFrame(HD3DRENDERER hRenderer,
					   const BYTE *pbyBuffer, int cbStride)
{
	if (hRenderer == NULL)
		return FALSE;

	MWDXRender *pRet = (MWDXRender *)hRenderer;
	return pRet->paint((unsigned char *)pbyBuffer);
}

HDSoundRENDERER LIBMWMEDIA_API MWCreateDSoundRenderer(int nSamplesPerSec,
						      int nChannels,
						      int nSamplesPerFrame,
						      int nBufferFrameCount)
{
	int ret = -1;

	CDSoundRenderer *pRender = new CDSoundRenderer();

	do {
		GUID guid = GUID_NULL;
		if (!pRender->Initialize(&guid))
			break;

		if (!pRender->Create(nSamplesPerSec, nChannels,
				     nSamplesPerFrame, nBufferFrameCount))
			break;

		if (!pRender->Run())
			break;

		ret = 0;
	} while (0);

	if (ret != 0) {
		pRender->Deinitialize();
		delete pRender;
		pRender = NULL;
	}

	return (HDSoundRENDERER)pRender;
}

void LIBMWMEDIA_API MWDestroyDSoundRenderer(HDSoundRENDERER hRenderer)
{
	if (hRenderer == NULL)
		return;

	CDSoundRenderer *pRender = (CDSoundRenderer *)hRenderer;
	pRender->Deinitialize();
	delete pRender;
}

BOOL LIBMWMEDIA_API MWDSoundRendererPushFrame(HDSoundRENDERER hRenderer,
					      const BYTE *pbFrame, int cbFram)
{
	if (hRenderer == NULL)
		return FALSE;

	CDSoundRenderer *pRender = (CDSoundRenderer *)hRenderer;

	return pRender->PutFrame(pbFrame, cbFram);
}
