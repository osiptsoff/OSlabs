#pragma once

#include <iostream>

#include <windows.h>
#include <fileapi.h>

using std::cout;
using std::wcin;

/*
	desypherFlags() receives a DWORD and prints
	all file attribute flags that DWORD contains
*/
void desypherFlags(DWORD flags) {
	if (flags & FILE_ATTRIBUTE_ARCHIVE) cout << "file is an archive;\n";
	if (flags & FILE_ATTRIBUTE_COMPRESSED) cout << "file is compressed;\n";
	if (flags & FILE_ATTRIBUTE_DEVICE) cout << "file is reserved for system use;\n";
	if (flags & FILE_ATTRIBUTE_DIRECTORY) cout << "file is a directory;\n";
	if (flags & FILE_ATTRIBUTE_ENCRYPTED) cout << "file is encrypted;\n";
	if (flags & FILE_ATTRIBUTE_HIDDEN) cout << "file is hidden;\n";
	if (flags & FILE_ATTRIBUTE_INTEGRITY_STREAM) cout << "file is configured with integrity;\n";
	if (flags & FILE_ATTRIBUTE_NORMAL) cout << "file has no special attributes;\n";
	if (flags & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) cout << "file not to be indexed by the content indexing service;\n";
	if (flags & FILE_ATTRIBUTE_NO_SCRUB_DATA) cout << "file is not to be read by the background data integrity scanner;\n";
	if (flags & FILE_ATTRIBUTE_OFFLINE) cout << "file is not available immediately;\n";
	if (flags & FILE_ATTRIBUTE_READONLY) cout << "file is readonly;\n";
	if (flags & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) cout << "file is not fully present locally;\n";
	if (flags & FILE_ATTRIBUTE_RECALL_ON_OPEN) cout << "file is virtual;\n";
	if (flags & FILE_ATTRIBUTE_REPARSE_POINT)cout << "file is a symbolic link;\n";
	if (flags & FILE_ATTRIBUTE_SPARSE_FILE) cout << "file is sparse;\n";
	if (flags & FILE_ATTRIBUTE_SYSTEM) cout << "system uses only a part of this file;\n";
	if (flags & FILE_ATTRIBUTE_TEMPORARY) cout << "file is being used for temporary storage;\n";
	if (flags & FILE_ATTRIBUTE_VIRTUAL) cout << "file is reserved for system use;\n";
	if (flags & FILE_ATTRIBUTE_PINNED) cout << "file should be kept fully present locally even when not being actively accessed;\n";
	if (flags & FILE_ATTRIBUTE_UNPINNED) cout << "file should not be kept fully present locally except when being actively accessed;\n";
}

/*
	fillTime() receives a pointer to SYSTEMTIME struct
	and fills it by content got from user input
*/
void fillTime(SYSTEMTIME* time) {
	WORD value;

	cout << "You are supposed to enter the time. Note that it must be UTC 0.\n";
	cout << "Enter a year (2012 for example): ";
	wcin >> value;
	wcin.ignore(INT_MAX, '\n');
	time->wYear = value;
	cout << "Enter a month (5 for example): ";
	wcin >> value;
	wcin.ignore(INT_MAX, '\n');
	time->wMonth = value;
	cout << "Enter a day (5 for example): ";
	wcin >> value;
	wcin.ignore(INT_MAX, '\n');
	time->wDay = value;
	cout << "Enter an hour (5 for example): ";
	wcin >> value;
	wcin.ignore(INT_MAX, '\n');
	time->wHour = value;
	cout << "Enter a minute (5 for example): ";
	wcin >> value;
	wcin.ignore(INT_MAX, '\n');
	time->wMinute = value;
	cout << "Enter a second (5 for example): ";
	wcin >> value;
	wcin.ignore(INT_MAX, '\n');
	time->wSecond = value;
}

/*
	processFileAttributes() prints information about file which
	path user provides through keyboard;
	after that, function provides an opportunity to
	change file flags and time
*/
void processFileAttributes() {
	_BY_HANDLE_FILE_INFORMATION fileInfo;
	SYSTEMTIME time;
	FILETIME fileTime;
	HANDLE file;
	wchar_t answer;
	wchar_t path[MAX_PATH];

	DWORD newFlags;

	cout << "\nDo you really want to work with file attributes? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');

	if (towupper(answer) != 'Y') return;

	// try to open file
	cout << "Enter the full name of file, max 255 symbols (D:\\Users\\Me\\file.txt for example): ";
	wcin.getline(path, MAX_PATH, L'\n');
	file = CreateFile(path,
		GENERIC_READ | FILE_WRITE_ATTRIBUTES,
		FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (file == INVALID_HANDLE_VALUE) {
		cout << "File does not exist." << std::endl;
		return;
	}

	// try to get info
	if (!GetFileInformationByHandle(file, &fileInfo)) {
		cout << "Cannot receive information about a file." << std::endl;
		return;
	}

	// desypher flags
	desypherFlags(fileInfo.dwFileAttributes);

	// print time
	FileTimeToSystemTime(&(fileInfo.ftCreationTime), &time);
	cout << "file was created at: " << time.wDay << "." << time.wMonth << "." << time.wYear
		<< ", " << time.wHour << "h : " << time.wMinute << "m : " << time.wSecond << "s UTC 0,\n";
	FileTimeToSystemTime(&(fileInfo.ftLastAccessTime), &time);
	cout << "file was last accessed at: " << time.wDay << "." << time.wMonth << "." << time.wYear
		<< ", " << time.wHour << "h : " << time.wMinute << "m : " << time.wSecond << "s UTC 0,\n";
	FileTimeToSystemTime(&(fileInfo.ftLastWriteTime), &time);
	cout << "file was last changed at: " << time.wDay << "." << time.wMonth << "." << time.wYear
		<< ", " << time.wHour << "h : " << time.wMinute << "m : " << time.wSecond << "s UTC 0,\n";

	// other info
	cout << "file is stored on a volume with serial number: " << fileInfo.dwVolumeSerialNumber;
	cout << ",\nfile size is: " << fileInfo.nFileSizeHigh << fileInfo.nFileSizeLow;
	cout << " bytes,\nnumber of links to this file: " << fileInfo.nNumberOfLinks;
	cout << ",\nfile identifier: " << fileInfo.nFileIndexHigh << fileInfo.nFileIndexLow << std::endl;

	// changing flags
	cout << ".\nDo you want to change file flags? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) == 'Y') {
		cout << "Enter the word of flags. If you have any questions,\nvisit https://learn.microsoft.com/en-us/windows/win32/fileio/file-attribute-constants: ";
		wcin >> newFlags;
		if (SetFileAttributes(path, newFlags))
			cout << "Flags are set.\n";
		else
			cout << "Failed to set flags.\n";
	}

	// changing time
	cout << "Do you want to change file creation time? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) == 'Y') {
		fillTime(&time);
		SystemTimeToFileTime(&time, &fileTime);
		if (SetFileTime(file, &fileTime, NULL, NULL))
			cout << "Time is set.\n";
		else
			cout << "Failed to set time.\n";
	}

	cout << "Do you want to change file last access time? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) == 'Y') {
		fillTime(&time);
		SystemTimeToFileTime(&time, &fileTime);
		if (SetFileTime(file, NULL, &fileTime, NULL))
			cout << "Time is set.\n";
		else
			cout << "Failed to set time.\n";
	}

	cout << "Do you want to change file last change time? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) == 'Y') {
		fillTime(&time);
		SystemTimeToFileTime(&time, &fileTime);
		if (SetFileTime(file, NULL, NULL, &fileTime))
			cout << "Time is set.\n";
		else
			cout << "Failed to set time.\n";
	}

	CloseHandle(file);
}