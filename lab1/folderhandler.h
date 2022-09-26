#pragma once

#include <iostream>

#include <windows.h>
#include <fileapi.h>

using std::cout;
using std::wcin;

/*
	makeFolder() creates a directory with path received from user if no such folder exists;
	path to folder should be full: D:\Users\Me\Folder for example,
	in this case in D:\Users\Me\ a new folder named Folder will be created;
	otherwise new folder can be created only in the same location with this program;
	this function does not work recursively (i.e. group of nested folders cannot be created by one call)
*/
void makeFolder() {
	wchar_t answer;
	wchar_t path[MAX_PATH];

	cout << "\nDo you really want to create new directory? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');

	if (towupper(answer) == 'Y') {
		cout << "Enter the full name of folder, max 255 symbols (D:\\Users\\Me\\NewFolder for example): ";
		wcin.getline(path, MAX_PATH, L'\n');

		if (CreateDirectory(path, NULL)) 
			cout << "Directory succsessfully created." << std::endl;
		else
			cout << "Error happened. Probably you entered incorrect name." << std::endl;
	}
}

/*
	removeFolder() removes a directory with path received from user;
	path to folder should be full: D:\Users\Me\Folder for example,
	in this case in D:\Users\Me\ a folder named Folder will be deleted;
	otherwise only directory located in the same place with this program will be removed;
	this function can remove only empty folders
*/
void removeFolder() {
	wchar_t answer;
	wchar_t path[MAX_PATH];

	cout << "\nDo you really want to delete a directory? 'y' for 'yes': ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');

	if (towupper(answer) == 'Y') {
		cout << "Enter the full name of folder, max 255 symbols (D:\\Users\\Me\\NewFolder for example): ";
		wcin.getline(path, MAX_PATH, L'\n');

		if (RemoveDirectory(path))
			cout << "Directory succsessfully removed." << std::endl;
		else
			cout << "Error happened. Probably you entered incorrect name or a folder was not empty." << std::endl;
	}
}