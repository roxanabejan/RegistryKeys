#include "stdafx.h"
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <iostream>
using namespace std;
typedef std::basic_string<TCHAR> tstring;
#pragma comment(lib, "User32.lib")

//HKEY newKey;
void DisplayErrorBox(LPTSTR lpszFunction);
HKEY createKey(STRSAFE_PCNZWCH path) {
	path = path + 3;
	TCHAR path2[MAX_PATH] = TEXT("Software\\CSSO");
	StringCchCat(path2, MAX_PATH, TEXT("\\"));
	StringCchCat(path2, MAX_PATH, path);
	HKEY hkey;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CURRENT_USER,
		path2,
		0, NULL, 0,
		KEY_WRITE, NULL,
		&hkey, &dwDisposition) == ERROR_SUCCESS) {

		_tprintf(TEXT("\nS-a creat o noua cheie.\n"));
	}
	return hkey;
}

int listare_recursiva(STRSAFE_PCNZWCH path, HKEY hKey) {
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	StringCchLength(path, MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		_tprintf(TEXT("\nDirectory path is too long.\n"));
		return (-1);
	}

	//_tprintf(TEXT("\nTarget directory is %s\n\n"), path);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{	
		TCHAR path2[MAX_PATH];
		StringCchCopy(path2, MAX_PATH, path);
		StringCchCat(path2, MAX_PATH, TEXT("\\"));
		StringCchCat(path2, MAX_PATH, ffd.cFileName);

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
			if (wcscmp(ffd.cFileName, L".") != 0 && wcscmp(ffd.cFileName, L"..") != 0)
			{
				_tprintf(TEXT("  %s   <path>\n"), path2);
				hKey = createKey(path2);
				cout << listare_recursiva(path2, hKey);
			}
				
				

		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			_tprintf(TEXT("  %s   <path>\n"), path2);
			DWORD fileSize = ffd.nFileSizeLow;
			long lRes = RegSetValueEx(hKey, ffd.cFileName, 0, REG_DWORD, (const BYTE*)&fileSize, sizeof(fileSize));
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	RegCloseKey(hKey);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
	}

	FindClose(hFind);
	//return dwError;

}

void DisplayErrorBox(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and clean up

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

int _tmain(int argc, TCHAR *argv[])
{


	// If the directory is not specified as a command-line argument,
	// print usage.

	if (argc != 2)
	{
		_tprintf(TEXT("\nUsage: %s <directory name>\n"), argv[0]);
		return (-1);
	}
	HKEY newKey = createKey(argv[1]);
	cout << listare_recursiva(argv[1], newKey);
	


}
