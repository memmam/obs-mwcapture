#ifndef IIDECODERSET_H
#define IIDECODERSET_H
#include <initguid.h>
#include <Unknwn.h>

// {19434C39-D9BA-47CA-84E0-BC9D2ABD9A1E}
DEFINE_GUID(IID_DecoderSet ,
            0x19434c39, 0xd9ba, 0x47ca, 0x84, 0xe0, 0xbc, 0x9d, 0x2a, 0xbd, 0x9a, 0x1e);

__interface IIDecoderSet:public IUnknown
{
STDMETHOD(GetFont)(WCHAR *t_wcFontPath) = 0;
STDMETHOD(SetFont)(WCHAR *t_wcFontPath) = 0;

STDMETHOD(GetFontSize)(int *t_nFontSize) = 0;
STDMETHOD(SetFontSize)(int t_nFontSize) = 0;

STDMETHOD(GetShow608_708)(bool *t_bShow608,bool *t_bShow708) = 0;
STDMETHOD(SetShow608_708)(bool t_nShow608,bool t_bShow708) = 0;

STDMETHOD(GetConnected)(bool *t_bConnected) = 0;

STDMETHOD(GetPicSize)(int *t_nWidth, int *t_nHeight) = 0;
STDMETHOD(SetPicSize)(int t_nWidth, int t_nHeight) = 0;

STDMETHOD(GetBacColor)(bool *t_bSet, int *r,int *g,int *b,int *a) = 0;
STDMETHOD(SetBacColor)(bool t_bSet, int r, int g, int b, int a) = 0;

STDMETHOD(GetFtcColor)(bool *t_bSet, int *r, int *g, int *b, int *a) = 0;
STDMETHOD(SetFtcColor)(bool t_bSet, int r, int g, int b, int a) = 0;

STDMETHOD(GetCC608Channel)(int *t_nField, int *t_nChannel) = 0;
STDMETHOD(SetCC608Channel)(int t_nField, int t_nChannel) = 0;
};

#endif