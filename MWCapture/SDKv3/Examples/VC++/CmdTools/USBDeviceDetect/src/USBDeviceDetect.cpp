/************************************************************************************************/
// USBDeviceDetect.cpp : Defines the entry point for the console application.

// MAGEWELL PROPRIETARY INFORMATION

// The following license only applies to head files and library within Magewell’s SDK 
// and not to Magewell’s SDK as a whole. 

// Copyrights © Nanjing Magewell Electronics Co., Ltd. (“Magewell”) All rights reserved.

// Magewell grands to any person who obtains the copy of Magewell’s head files and library 
// the rights,including without limitation, to use, modify, publish, sublicense, distribute
// the Software on the conditions that all the following terms are met:
// - The above copyright notice shall be retained in any circumstances.
// -The following disclaimer shall be included in the software and documentation and/or 
// other materials provided for the purpose of publish, distribution or sublicense.

// THE SOFTWARE IS PROVIDED BY MAGEWELL “AS IS” AND ANY EXPRESS, INCLUDING BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL MAGEWELL BE LIABLE 

// FOR ANY CLAIM, DIRECT OR INDIRECT DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT,
// TORT OR OTHERWISE, ARISING IN ANY WAY OF USING THE SOFTWARE.

// CONTACT INFORMATION:
// SDK@magewell.net
// http://www.magewell.com/
//
/************************************************************************************************/

#include "stdafx.h"
#include <stdio.h>
#include "LibMWCapture/MWCapture.h"
#include "LibMWCapture/MWUSBCapture.h"

// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

HCHANNEL hChannel;
int		 *nUsbDevice;
int		 nChannelCount;

void		CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG		ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void        ShowInfo();
void        Initialize();
void        USBDeviceDetect();
static void	HotplugCheckCallback(MWCAP_USBHOT_PLUG_EVENT event, const char *pszDevicePath, void* pParam);
void        ExitProgram();

int main(int argc, char* argv[])
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler); 

	ShowInfo();

	Initialize();

	USBDeviceDetect();

	ExitProgram();

	return 0;
}

//show the basic information of our capture card
void ShowInfo()
{
	BYTE byMaj, byMin;
	WORD wBuild;
	MWGetVersion(&byMaj, &byMin, &wBuild);

	printf("Magewell MWCapture SDK %d.%d.1.%d - USBDeviceDetect\n", byMaj, byMin, wBuild);
	printf("Only USB devices are supported\n");
	printf("Usage:\n");
	printf("USBDeviceDetect.exe\n");
}

void Initialize()
{
	hChannel = NULL;
	nUsbDevice = NULL;
	nChannelCount=0;

	MWCaptureInitInstance();
}

void USBDeviceDetect()
{
	MW_RESULT mr = MW_SUCCEEDED;

	mr = MWUSBRegisterHotPlug(HotplugCheckCallback, NULL);
	if (mr != MW_SUCCEEDED)
	{
		printf("Set usb device detect event failed\n");
		return;
	}

	printf("Please disconnect and reconnect the specific usb device\n");
	printf("It will listen for USB devices connection change events for 10 seconds.\n");
	for(int i=0;i<10;i++){
		printf("%ds\n",i);
		Sleep(1000);
	}

	MWUSBUnRegisterHotPlug();
}

static void HotplugCheckCallback(MWCAP_USBHOT_PLUG_EVENT event, const char *pszDevicePath, void* pParam)
{
	switch(event)
	{
	case MWCAP_USBHOT_PLUG_EVENT_DEVICE_ARRIVED:
		printf("Device reconnect\n");
		break;
	case MWCAP_USBHOT_PLUG_EVENT_DEVICE_LEFT:
		printf("Device disconnect\n");
		break;
	default:
		break;
	}
}

void ExitProgram(){

	fflush(stdin);
	printf("Press \"Enter\" key to exit...\n");
	getchar();

	MWCaptureExitInstance();
}

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
{  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
	dumpInfo.ExceptionPointers = pException;  
	dumpInfo.ThreadId = GetCurrentThreadId();  
	dumpInfo.ClientPointers = TRUE;  

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  
	CloseHandle(hDumpFile);  
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
{     
	CreateDumpFile(L"USBDeviceDetect.dmp",pException);  
	return EXCEPTION_EXECUTE_HANDLER;  
}
