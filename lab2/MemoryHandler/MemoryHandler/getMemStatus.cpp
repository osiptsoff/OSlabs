#include <iostream>
#include <windows.h>
#include <fileapi.h>

#include "memoryhandler.h"

using std::cout;

void getMemStatus() {
	LPMEMORYSTATUSEX memInfo;
	wchar_t answer;

	// confirming action
	cout << "Do you really want to get information about memory? ('y' for 'yes'): ";
	std::wcin >> answer;
	std::wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) != 'Y')
		return;

	// getting memory info
	memInfo = new MEMORYSTATUSEX();
	memInfo->dwLength = sizeof(*memInfo);
	if (!GlobalMemoryStatusEx(memInfo)) {
		cout << "Error has occured.\n";
		return;
	}

	// printing memory info
	cout << "Used " << memInfo->dwMemoryLoad << "% of memory;\n";
	cout << "Actual physical memory: " << memInfo->ullTotalPhys << "\n";
	cout << "Available physical memory: " << memInfo->ullAvailPhys << "\n";
	cout << "The current committed memory limit for the system or the current process: " << memInfo->ullTotalPageFile << "\n";
	cout << "The maximum amount of memory the current process can commit: " << memInfo->ullAvailPageFile << "\n";
	cout << "Total size of user-mode portion of virtual memory this process can use: " << memInfo->ullTotalVirtual << "\n";
	cout << "Size of uncommited user-mode portion of virtual memory this process can use: " << memInfo->ullAvailVirtual << "\n";

	// freeing used memory
	delete memInfo;
}