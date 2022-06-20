
#pragma once

#ifdef LIBMWMEDIA_EXPORTS
#define LIBMWMEDIA_API __declspec(dllexport)
#else
#define LIBMWMEDIA_API __declspec(dllimport)
#endif

#include <Windows.h>
#include <tchar.h>


#ifdef __cplusplus

extern "C"
{
#endif

/////////////////////////
// D3D 11 Renderer

DECLARE_HANDLE(HD3DRENDERER);
DECLARE_HANDLE(HDSoundRENDERER);

bool
LIBMWMEDIA_API
MWGetD3D9RenderSupportFormat(
	int*					pnFmtNum,
	DWORD*					pdwFmt
);

HD3DRENDERER
LIBMWMEDIA_API
MWCreateD3DRenderer(
	int						cx,
	int						cy,
	DWORD					dwFourcc,
	bool					bReverse,
	HWND					hWnd
	);

void
LIBMWMEDIA_API
MWDestroyD3DRenderer(
	HD3DRENDERER			hRenderer
	);

BOOL
LIBMWMEDIA_API
MWD3DRendererPushFrame(
	HD3DRENDERER			hRenderer,
	const BYTE *			pbyBuffer,
	int						cbStride
	);


HDSoundRENDERER 
	LIBMWMEDIA_API
	MWCreateDSoundRenderer(
	int nSamplesPerSec, 
	int nChannels, 
	int nSamplesPerFrame, 
	int nBufferFrameCount
	);

void
	LIBMWMEDIA_API
	MWDestroyDSoundRenderer(
	HDSoundRENDERER	hRenderer
	);

BOOL
	LIBMWMEDIA_API
	MWDSoundRendererPushFrame(
	HDSoundRENDERER hRenderer,
	const BYTE  *pbFrame, 
	int cbFram
	);

#ifdef __cplusplus
}
#endif
