// ReadWriteEDID.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>

#include "LibMWCapture\MWCapture.h"

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Magewell MWCapture SDK 3.2 - ResetEDID\n");

	MWCaptureInitInstance();

	// Version
	HCHANNEL hChannel = NULL;

	do {
		int nChannelCount = MWGetChannelCount();
		if (nChannelCount == 0) {
			printf("ERROR: Can't find channels!\n");
			break;
		}
		printf("Find %d channels.\n", nChannelCount);

		// Get board id and channel id
		MWCAP_CHANNEL_INFO videoInfo = { 0 };
		for (int i = 0; i < nChannelCount; i ++) {
			TCHAR szPath[MAX_PATH];
			if (MW_SUCCEEDED != MWGetDevicePath(i, szPath)) {
				printf("ERROR: Can't get channel info - %d!\n", i);
				continue;
			}

			// Open channel
			hChannel = MWOpenChannelByPath(szPath);
			if (hChannel == NULL) {
				printf("ERROR: Open channel %d error!\n", i);
				break;
			}

			if (MW_SUCCEEDED != MWSetEDID(hChannel, NULL, 0)) {
				printf("ERROR: Reset EDID - %d!\n", i);
			}
			else {
				printf("Reset EDID %d ok!\n", i);
			}

			MWCloseChannel(hChannel);
		}
		
	} while (FALSE);
	
	MWCaptureExitInstance();

	return 0;
}

