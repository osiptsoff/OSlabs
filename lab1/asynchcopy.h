#pragma once

#include <iostream>

#include <windows.h>
#include <fileapi.h>

using std::cout;
using std::wcin;

// number of overlapped IO operations
const int OverlappingOperationsAmount = 16;
// multiplier used for scaling size of block transferred for one IO operation
const int BlockSizeClustersMultiplier = 16;
// boolean value used to define whether assigning filesize before writing is needed
const bool setSizeBeforeWriting = true;

bool enablePrivileges()
{
	HANDLE token;

	struct {
		DWORD count;
		LUID_AND_ATTRIBUTES privilege[3];
	} token_privileges;

	token_privileges.count = 1;
	token_privileges.privilege[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!LookupPrivilegeValue(0, SE_MANAGE_VOLUME_NAME, &token_privileges.privilege[0].Luid)) return false;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token)) return false;
	if (!AdjustTokenPrivileges(token, 0, (PTOKEN_PRIVILEGES)&token_privileges, 0, 0, 0)) return false;

	return true;
}

/*
	this struct is designed for keeping sizes of files
	and offsets in one variable with ability to take
	their higher and lesser words
*/
typedef union {
	unsigned long long total;
	struct {
		DWORD lesser;
		DWORD higher;
	};
} size;

/*
	this struct is designed for passing buffer
	and offset to callback routines
	(pointer to instance is assigned to unused hEvent field of OVERLAPPED)
*/
typedef struct {
	void* buffer;
	size currentOffset;
} threadProps;

HANDLE src, dst;
size filesize, clustersize;

void _stdcall afterWriteRoutine(DWORD errorCode, DWORD bytesTransfered, LPOVERLAPPED props);
void _stdcall afterReadRoutine(DWORD errorCode, DWORD bytesTransfered, LPOVERLAPPED props);

/*
	routine called after write operation;
	here we determine if we need further copying in current thread
	and arrange it if needed
*/
void _stdcall afterWriteRoutine(DWORD errorCode, DWORD bytesTransfered, LPOVERLAPPED props) {
	// threadProps instance is passed through hEvent, so we extract it
	threadProps* tprop = static_cast<threadProps*>(props->hEvent);

	// grow offset
	tprop->currentOffset.total += clustersize.total * OverlappingOperationsAmount;
	props->Offset = tprop->currentOffset.lesser;
	props->OffsetHigh = tprop->currentOffset.higher;

	// we need further copying only if grown offset is within file borders
	if (filesize.total > tprop->currentOffset.total)
		ReadFileEx(src,
			tprop->buffer,
			clustersize.total,
			props,
			afterReadRoutine);
}

/*
	routine called after read operation;
	here we write read data to copyfile
*/
void _stdcall afterReadRoutine(DWORD errorCode, DWORD bytesTransfered, LPOVERLAPPED props) {
	// threadProps instance is passed through hEvent, so we extract it
	threadProps* tprop = static_cast<threadProps*>(props->hEvent);

	WriteFileEx(dst,
		tprop->buffer,
		bytesTransfered,
		props,
		afterWriteRoutine);
}

/*
	copyFileAsynch() copies file in OverlappingOperationsAmount threads
	that perform overlapped IO operations
*/
void copyFileAsynch() {
	wchar_t srcPath[MAX_PATH], dstPath[MAX_PATH], srcRoot[] = L"A:\\";
	wchar_t answer;

	threadProps tprops[OverlappingOperationsAmount];
	OVERLAPPED props[OverlappingOperationsAmount];

	DWORD sectorPerCluster, bytesPerSector, mock, startTime, endTime;

	cout << "\nDo you really want to copy file asyncronously? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');

	if (towupper(answer) != 'Y') return;

	// determine the source
	cout << "Enter the full name of file, max 255 symbols (D:\\Users\\Me\\file.txt for example): ";
	wcin.getline(srcPath, MAX_PATH, L'\n');

	// determine the destination
	cout << "Enter the name of copy, max 255 symbols (D:\\Users\\Me\\newfile.txt for example): ";
	wcin.getline(dstPath, MAX_PATH, L'\n');

	if (setSizeBeforeWriting)
		enablePrivileges();

	// open files
	src = CreateFile(srcPath,
		FILE_GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
		NULL);
	dst = CreateFile(dstPath,
		FILE_GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_NEW,
		FILE_FLAG_OVERLAPPED,
		NULL);

	// check if opened correctly
	if (src == INVALID_HANDLE_VALUE || dst == INVALID_HANDLE_VALUE) {
		cout << "Original file was not opened or copy was not created.\n"
			<< "Check if paths are correct, original exists and copy does not." << std::endl;
		CloseHandle(src);
		CloseHandle(dst);
		SetLastError(0);
		return;
	}

	// determine the size
	filesize.lesser = GetFileSize(src, &filesize.higher);
	// set the destination filesize equal to source filesize if required
	if (setSizeBeforeWriting) {
		LARGE_INTEGER liFilesize, liZero;
		liFilesize.QuadPart = filesize.total + 1;
		liZero.QuadPart = 0;

		if (!(SetFilePointerEx(dst, liFilesize, NULL, 0) &&
				SetEndOfFile(dst) && 
				SetFilePointerEx(dst, liZero, NULL, 0) && 
				SetFileValidData(dst, filesize.total) ) ) {
			cout << "Failed to set destination filesize. Error " << GetLastError() << " .\n";
			SetLastError(0);
			CloseHandle(src);
			CloseHandle(dst);
			return;
		}
	}

	// determine the clustersize
	srcRoot[0] = srcPath[0];
	// unused output values are written in single dummy
	GetDiskFreeSpace(srcRoot, &sectorPerCluster, &bytesPerSector, &mock, &mock);
	clustersize.total = sectorPerCluster * bytesPerSector * BlockSizeClustersMultiplier;

	startTime = GetTickCount();

	for (int i = 0; i < OverlappingOperationsAmount; ++i) {
		tprops[i].buffer = malloc(clustersize.total + 1);
		tprops[i].currentOffset.total = i * clustersize.total;

		props[i].Internal = 0;
		props[i].InternalHigh = 0;
		props[i].Pointer = 0;
		props[i].Offset = tprops[i].currentOffset.lesser;
		props[i].OffsetHigh = tprops[i].currentOffset.higher;
		
		props[i].hEvent = tprops + i;

		ReadFileEx(src,
			tprops[i].buffer,
			clustersize.total,
			props + i,
			afterReadRoutine);
	}

	// wait for operations to end
	while (SleepEx(100, true) );

	endTime = GetTickCount();
	cout << "Completed. Operation took " << endTime - startTime << " ms." << std::endl;

	// close files, free buffers
	CloseHandle(src);
	CloseHandle(dst);
	for (int i = 0; i < OverlappingOperationsAmount; ++i) free(tprops[i].buffer);
}