/************************************************************************************************/
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

#pragma once

#include <Windows.h>
#include <MMSystem.h>
#include <Mmreg.h.>

class CWaveFile
{
public:
	CWaveFile(void) {
		m_pFile = NULL;
		bMultiChannel = FALSE;
	}
	virtual ~CWaveFile(void) {
		if (m_pFile != NULL) {
			Exit();
		}
	}

public:
	BOOL Init(const CHAR * pszFilePath, int nSamplesPerSec, int nChannels = 2, WORD wBitsPerSample = 16)
	{
		bMultiChannel = (nChannels > 2) ? TRUE : FALSE;

		if (bMultiChannel) {

			m_wfHeaderEx.dwRiffFlag = 'FFIR';
			m_wfHeaderEx.dwFileSize = sizeof(WAVE_FILE_HEADER_EX) - 12;

			m_wfHeaderEx.dwWaveFlag = 'EVAW';
			m_wfHeaderEx.dwFmtFlag = ' tmf';
			m_wfHeaderEx.dwFmtSize = sizeof(WAVEFORMATEXTENSIBLE);

			m_wfHeaderEx.fmtEx.Format.wBitsPerSample = wBitsPerSample;
			m_wfHeaderEx.fmtEx.Format.wFormatTag = nChannels > 2 ? 0x00FFFE : WAVE_FORMAT_PCM;
			m_wfHeaderEx.fmtEx.Format.nChannels = nChannels;
			m_wfHeaderEx.fmtEx.Format.nSamplesPerSec = nSamplesPerSec;
			m_wfHeaderEx.fmtEx.Format.nBlockAlign = m_wfHeaderEx.fmtEx.Format.wBitsPerSample * nChannels / 8;
			m_wfHeaderEx.fmtEx.Format.nAvgBytesPerSec = m_wfHeaderEx.fmtEx.Format.nBlockAlign * nSamplesPerSec;

			m_wfHeaderEx.fmtEx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

			m_wfHeaderEx.fmtEx.Samples.wValidBitsPerSample = wBitsPerSample;
			switch(nChannels) {
			case 1:
				m_wfHeaderEx.fmtEx.dwChannelMask = (SPEAKER_FRONT_CENTER);
				break;
			case 2:
				m_wfHeaderEx.fmtEx.dwChannelMask = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT);
				break;
			case 3:
				m_wfHeaderEx.fmtEx.dwChannelMask = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER); 
				// (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_CENTER);
				break;
			case 4:
				m_wfHeaderEx.fmtEx.dwChannelMask = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT); 
				//(SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER);
				break;
			case 5:
				m_wfHeaderEx.fmtEx.dwChannelMask = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT);
				break;
			case 6:
				m_wfHeaderEx.fmtEx.dwChannelMask = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_LOW_FREQUENCY);
				break;
			case 7:
				m_wfHeaderEx.fmtEx.dwChannelMask = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT);
				break;
			case 8:
				m_wfHeaderEx.fmtEx.dwChannelMask = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT | SPEAKER_LOW_FREQUENCY);
				break;
			}

			m_wfHeaderEx.fmtEx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

			m_wfHeaderEx.dwDataFlag = 'atad';
			m_wfHeaderEx.dwDataSize = 0;

			fopen_s(&m_pFile, pszFilePath, "wb");
			if (NULL == m_pFile)
				return FALSE;

			if (fwrite(&m_wfHeaderEx, sizeof(WAVE_FILE_HEADER_EX), 1, m_pFile) != 1) {
				fclose(m_pFile);
				return FALSE;
			}
		}
		else {
			m_wfHeader.dwRiffFlag = 'FFIR';
			m_wfHeader.dwFileSize = sizeof(WAVE_FILE_HEADER) - 12;

			m_wfHeader.dwWaveFlag = 'EVAW';
			m_wfHeader.dwFmtFlag = ' tmf';
			m_wfHeader.dwFmtSize = sizeof(PCMWAVEFORMAT);

			m_wfHeader.pcmFormat.wBitsPerSample = wBitsPerSample;
			m_wfHeader.pcmFormat.wf.wFormatTag = WAVE_FORMAT_PCM;
			m_wfHeader.pcmFormat.wf.nChannels = nChannels;
			m_wfHeader.pcmFormat.wf.nSamplesPerSec = nSamplesPerSec;
			m_wfHeader.pcmFormat.wf.nBlockAlign = m_wfHeader.pcmFormat.wBitsPerSample * nChannels / 8;
			m_wfHeader.pcmFormat.wf.nAvgBytesPerSec = m_wfHeader.pcmFormat.wf.nBlockAlign * nSamplesPerSec;

			m_wfHeader.dwDataFlag = 'atad';
			m_wfHeader.dwDataSize = 0;

			fopen_s(&m_pFile, pszFilePath, "wb");
			if (NULL == m_pFile)
				return FALSE;

			if (fwrite(&m_wfHeader, sizeof(WAVE_FILE_HEADER), 1, m_pFile) != 1) {
				fclose(m_pFile);
				return FALSE;
			}
		}
		return TRUE;
	}

	void Exit()
	{
		if (m_pFile) {
			fseek(m_pFile, 0, SEEK_SET);

			if (bMultiChannel) {
				m_wfHeaderEx.dwFileSize = m_wfHeaderEx.dwDataSize + sizeof(WAVE_FILE_HEADER_EX) - 12;
				fwrite(&m_wfHeaderEx, sizeof(m_wfHeaderEx), 1, m_pFile);
			}
			else {
				m_wfHeader.dwFileSize = m_wfHeader.dwDataSize + sizeof(WAVE_FILE_HEADER) - 12;
				fwrite(&m_wfHeader, sizeof(m_wfHeader), 1, m_pFile);
			}

			fclose(m_pFile);
			m_pFile = NULL;
		}
	}

	BOOL Write(const BYTE * pData, int nSize)
	{
		if (fwrite(pData, nSize,1,  m_pFile) != 1)
			return FALSE;

		if (bMultiChannel)
			m_wfHeaderEx.dwDataSize += nSize;
		else
			m_wfHeader.dwDataSize += nSize;
		return TRUE;
	}
	BOOL IsOpen() {
		return (m_pFile != NULL);
	}

protected:
	BOOL bMultiChannel;
	FILE * m_pFile;

	typedef struct tagWAVE_FILE_HEADER {
		DWORD dwRiffFlag; // 'RIFF'
		DWORD dwFileSize;
		DWORD dwWaveFlag; // 'WAVE'
		DWORD dwFmtFlag;  // 'fmt'
		DWORD dwFmtSize;

		PCMWAVEFORMAT pcmFormat;

		DWORD dwDataFlag; // 'data'
		DWORD dwDataSize;
	} WAVE_FILE_HEADER, *PWAVE_FILE_HEADER;

	WAVE_FILE_HEADER m_wfHeader;
	
	typedef struct tagWAVE_FILE_HEADER_EX {
		DWORD dwRiffFlag; // 'RIFF'
		DWORD dwFileSize;
		DWORD dwWaveFlag; // 'WAVE'
		DWORD dwFmtFlag;  // 'fmt'
		DWORD dwFmtSize;

		WAVEFORMATEXTENSIBLE fmtEx;

		DWORD dwDataFlag; // 'data'
		DWORD dwDataSize;
	} WAVE_FILE_HEADER_EX, *PWAVE_FILE_HEADER_EX;
	
	WAVE_FILE_HEADER_EX m_wfHeaderEx;
};