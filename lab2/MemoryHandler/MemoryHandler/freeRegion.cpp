#include <iostream>

#include "memoryhandler.h"
#include <windows.h>
#include <fileapi.h>

using std::cout;
using std::wcin;

void freeRegion() {
	MEMORY_BASIC_INFORMATION memInfo;
	PVOID address = nullptr;
	wchar_t answer;

	// confirming action
	cout << "Do you really want to free a memory region? ('y' for 'yes'): ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) != 'Y')
		return;

	// specify region
	cout << "Enter the address belonging to region you want to free: ";
	wcin >> std::hex >> address >> std::dec;
	wcin.ignore(INT_MAX, '\n');
	if (address == nullptr) {
		cout << "Failed to parse address.\n";
		return;
	}

	// get info about region
	VirtualQuery(address, &memInfo, sizeof(memInfo));

	// check for mistakes
	if (GetLastError() != 0) {
		cout << "Failed. Error " << GetLastError() << ".\n";
		SetLastError(0);
		return;
	}

	// free region
	cout << "You will free region. starting from "
		<< memInfo.BaseAddress
		<< ", with size of " << memInfo.RegionSize << " bytes.\n";
	if (VirtualFree(memInfo.BaseAddress, 0, MEM_RELEASE))
		cout << "Succsess.\n";
	else {
		cout << "Fail."
			<< " Error: " << GetLastError() << ".\n"
			<< "Possible reason: memory was not allocated.\n";
		SetLastError(0);
	}
}