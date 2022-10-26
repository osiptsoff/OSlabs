#include <iostream>
#include <windows.h>
#include <fileapi.h>

#include "memoryhandler.h"

using std::cout;
using std::wcin;

void protectRegion() {
	MEMORY_BASIC_INFORMATION memInfo;
	PVOID address = nullptr;
	DWORD newProtection, oldProtection;
	wchar_t answer;

	// confirming action
	cout << "Do you really want to change protection of memory region? ('y' for 'yes'): ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) != 'Y')
		return;

	// specify region
	cout << "Enter the address belonging to region you want to change protection of: ";
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

	// change protection
	cout << "You will change protection of region, starting from "
		<< memInfo.BaseAddress
		<< ", with size of " << memInfo.RegionSize << " bytes."
		<< "\nEnter new word of protection flags (hex)"
		<< "\n(info: https://learn.microsoft.com/en-us/windows/win32/memory/memory-protection-constants): ";
	wcin >> std::hex >> newProtection >> std::dec;
	wcin.ignore(INT_MAX, '\n');
	if (VirtualProtect(memInfo.BaseAddress, memInfo.RegionSize, newProtection, &oldProtection))
		cout << "Protection succsefffully changed.\n"
			<< "Old protection word: " << oldProtection << ".\n";
	else {
		cout << "Protection change failed."
			<< " Error: " << GetLastError() << ".\n"
			<< "Possible reasons: memory was not allocated or incorrect protection word.\n";
		SetLastError(0);
	}
}