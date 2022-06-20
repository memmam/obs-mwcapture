/************************************************************************************************/
// Mp4Repair.cpp : Defines the entry point for the console application.

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
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <windows.h>
#include "LibMWCapture\MWCapture.h"
#include "LibMWMp4/mw_mp4.h"

// for dump info
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName,
		    EXCEPTION_POINTERS *pException);
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void print_useage();
void exit_program(int t_n_code);

void show_info()
{
	BYTE byMaj, byMin;
	WORD wBuild;
	MWGetVersion(&byMaj, &byMin, &wBuild);

	printf("Magewell MWCapture SDK %d.%d.1.%d - Mp4Repair\n", byMaj, byMin,
	       wBuild);
	printf("Usage:\n");
	print_useage();
}

void print_useage()
{
	printf("Mp4Repair [filename]/[dir]\n");
}

int repair_file(char *p_mp4_file, char *p_info_file)
{
	mw_mp4_status_t status = mw_mp4_repair(p_mp4_file, true);
	if (status != MW_MP4_STATUS_SUCCESS) {
		printf("repair %s fail\n", p_mp4_file);
		return 1;
	}
	printf("repair %s success\n", p_mp4_file);
	return 0;
}

int repair_file_search(char *p_path, char *p_file_name)
{
	char mp4_file[256];
	char info_file[256];
	int len;
	sprintf_s(info_file, "%s/%s", 256, p_path, p_file_name);
	len = strlen(info_file);
	if (len <= 260) {
		memcpy(mp4_file, info_file, len - 5);
		mp4_file[len - 5] = 0;
		return repair_file(mp4_file, info_file);
	} else {
		return -1;
	}
}

int repair_dir(char *p_path)
{
	char file[256];
	WIN32_FIND_DATA find_data;
	sprintf_s(file, "%s\\*.mp4.info*", p_path);
	HANDLE dir = FindFirstFile(file, &find_data);
	if (INVALID_HANDLE_VALUE != dir) {
		repair_file_search(p_path, find_data.cFileName);
	} else {
		return 0;
	}
	while (FindNextFile(dir, &find_data)) {
		repair_file_search(p_path, find_data.cFileName);
	}
	FindClose(dir);
	return 0;
}
int main(int argc, char *argv[])
{
	SetUnhandledExceptionFilter(
		(LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	int str_len;
	char info_file[256];
	if (argc < 2) {
		show_info();
		exit_program(-1);
	}
	show_info();
	str_len = strlen(argv[1]);
	if (0 == memcmp(argv[1] + str_len - 4, ".mp4", 4)) {
		sprintf_s(info_file, "%s.info", argv[1]);
		return repair_file(argv[1], info_file);
	}
	int t_n_ret = repair_dir(argv[1]);
	exit_program(t_n_ret);
}

void exit_program(int t_n_code)
{
	printf("Press \"Enter\" key to exit...\n");
	getchar();
	exit(t_n_code);
}

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName,
		    EXCEPTION_POINTERS *pException)
{
	HANDLE hDumpFile = CreateFileW(lpstrDumpFilePathName, GENERIC_WRITE, 0,
				       NULL, CREATE_ALWAYS,
				       FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
			  MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	CreateDumpFile(L"Mp4Repair.dmp", pException);
	return EXCEPTION_EXECUTE_HANDLER;
}
