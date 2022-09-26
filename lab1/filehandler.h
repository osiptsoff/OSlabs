#pragma once

#include <iostream>

#include <windows.h>
#include <fileapi.h>

using std::cout;
using std::wcin;

/*
	createNewFile() allows user to create file in specified location
	and determine attributes: access rights, sharing mode;
	this function does not work recursively (i.e. group of nested folders cannot be created to contain new file)
*/
void createNewFile() {
	DWORD rights = 0, accessMode = 0;
	HANDLE result;
	wchar_t answer;
	wchar_t path[MAX_PATH];

	cout << "\nDo you really want to create new file? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');

	if (towupper(answer) == 'Y') {
		// determine the path
		cout << "Enter the full name of file, max 255 symbols (D:\\Users\\Me\\file.txt for example): ";
		wcin.getline(path, MAX_PATH, L'\n');

		// determine the access rights
		cout << "Enter the access rights to file ('r_' for readonly, '_w' for writeonly, 'rw' for read and write, no quotes),\n"
			<< "if any incorrect letter will be got from specified position, if will be treated as '_'.\n"
			<< "Your choice: ";
		wcin >> answer;
		if (answer == 'r') rights |= GENERIC_READ;
		wcin >> answer;
		if (answer == 'w') rights |= GENERIC_WRITE;
		wcin.ignore(INT_MAX, '\n');

		// determine the sharing mode
		cout << "Enter the sharing mode to determine if other processes may work with new file:\n"
			<< "'___' - no sharing, only this app will interact with file,\n"
			<< "'d__' - share deletion and changing name,\n"
			<< "'_r_' - share reading,\n"
			<< "'__w' - whare writing,\n"
			<< "you can also enter any combination of modes ('d_w' or 'drw' for example), but keep the order.\n"
			<< "if any incorrect letter will be got from specified position, if will be treated as '_'"
			<< "Your choice: ";
		wcin >> answer;
		if (answer == 'd') rights |= FILE_SHARE_DELETE;
		wcin >> answer;
		if (answer == 'r') rights |= FILE_SHARE_READ;
		wcin >> answer;
		if (answer == 'w') rights |= FILE_SHARE_WRITE;
		wcin.ignore(INT_MAX, '\n');

		// create file and warn about mistakes, if any
		result = CreateFile(path, rights, accessMode, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (result != INVALID_HANDLE_VALUE) {
			CloseHandle(result);
			cout << "File created succsessfully." << std::endl;
			return;
		}

		if (GetLastError() == ERROR_FILE_EXISTS) 
			cout << "File with this name already exists." << std::endl;
		else
			cout << "Error has occured. Did you write correct filename?" << std::endl;
	};
}

/*
	moveFile() allows user to move file from one location to another
	with possibility to give a new name to file;
	function tracks down attempts to overwrite an existing file and prevents them
*/
void moveFile() {
	wchar_t answer;
	wchar_t src[MAX_PATH], dst[MAX_PATH];


	cout << "\nDo you really want to move file? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');

	if (towupper(answer) == 'Y') {
		// determine the source
		cout << "Enter the full name of file, max 255 symbols (D:\\Users\\Me\\file.txt for example): ";
		wcin.getline(src, MAX_PATH, L'\n');

		// determine the destination
		cout << "Enter the new name of file, max 255 symbols (D:\\Users\\Me\\newfile.txt for example): ";
		wcin.getline(dst, MAX_PATH, L'\n');

		// move the file
		if (MoveFile(src, dst)) 
			cout << "Succsess." << std::endl;
		else if (GetLastError() == 183L)
			cout << "Destination file already exists." << std::endl;
		else
			cout << "Error. Did you enter correct paths?" << std::endl;
	}
}

/*
	copyFile() allows user to copy file
	with possibility to give a new name to copy;
	function tracks down attempts to overwrite an existing file and prevents it
*/
void copyFile() {
	wchar_t answer;
	wchar_t src[MAX_PATH], dst[MAX_PATH];


	cout << "\nDo you really want to copy file? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');

	if (towupper(answer) == 'Y') {
		// determine the source
		cout << "Enter the full name of file, max 255 symbols (D:\\Users\\Me\\file.txt for example): ";
		wcin.getline(src, MAX_PATH, L'\n');

		// determine the destination
		cout << "Enter the name of copy, max 255 symbols (D:\\Users\\Me\\newfile.txt for example): ";
		wcin.getline(dst, MAX_PATH, L'\n');

		// copy the file
		if (CopyFile(src, dst, true))
			cout << "Succsess." << std::endl;
		else if (GetLastError() == 80L)
			cout << "Destination file already exists." << std::endl;
		else
			cout << "Error. Did you enter correct paths?" << std::endl;
	}
}
