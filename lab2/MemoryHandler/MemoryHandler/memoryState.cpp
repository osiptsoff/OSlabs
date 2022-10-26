#include <iostream>
#include <windows.h>
#include <fileapi.h>

#include "memoryhandler.h"

using std::cout;
using std::wcin;

void memoryState() {
	MEMORY_BASIC_INFORMATION memInfo;
	PVOID address = nullptr;
	wchar_t answer;

	// confirming action
	cout << "Do you really want to get information about specific region of memory? ('y' for 'yes'): ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) != 'Y')
		return;

	// specify region
	cout << "Enter the address belonging to region you want to retrieve info about (hex): ";
	wcin >> std::hex >> address >> std::dec;
	wcin.ignore(INT_MAX, '\n');
	if (address == nullptr) {
		cout << "Failed to parse address.\n";
		return;
	}

	// get required info
	VirtualQuery(address, &memInfo, sizeof(memInfo));

	// check for mistakes
	if (GetLastError() != 0) {
		cout << "Failed to get info. Error " << GetLastError() << ".\n";
		SetLastError(0);
		return;
	}

	// print info
	cout << "Base address of desired region: " << memInfo.BaseAddress << ";\n";
	cout << "Base address of all memory allocated by this process: " << memInfo.AllocationBase << ";\n";
	cout << "Word of region's protection option it was allocated with (hex): " << std::hex << memInfo.AllocationProtect << std::endl
		<< "\t(more details: https://learn.microsoft.com/en-us/windows/win32/memory/memory-protection-constants)"
		<< ";\n";
	cout << "Word of region's current protection option (hex): " << std::hex << memInfo.Protect << std::endl;
	cout << "Size of the region (dec): " << std::dec << memInfo.RegionSize << ";" << std::endl;
	switch (memInfo.State) {
		case MEM_COMMIT:
			cout << "Physical storage was allocated for this region;\n";
			break;
		case MEM_FREE:
			cout << "This memory was not reserved neither has not been target of commit;\n";
			break;
		case MEM_RESERVE:
			cout << "Region is reserved with no physical storage;\n";
			break;
		default:
			cout << "Memory state: ERROR;\n";
	}
	switch (memInfo.Type) {
		case MEM_IMAGE:
			cout << "Memory pages within the region are mapped into the view of an image section;\n";
			break;
		case MEM_MAPPED:
			cout << "Memory pages within the region are mapped into the view of a section;\n";
			break;
		case MEM_PRIVATE:
			cout << "Memory pages within the region are private (that is, not shared by other processes);\n";
			break;
		default:
			cout << "Memory type: ERROR;\n";
	}

	// ask if region listing is required
	cout << "Do you want to see the content of this region? ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) != 'Y')
		return;

	// check if reading is allowed
	// 0x66 is a word containing all protection flags that allow reading
	if (!(0x66 & memInfo.AllocationProtect) || memInfo.State != MEM_COMMIT) {
		cout << "Reading of this region is not allowed.\n";
		return;
	}

	// show region content
	cout << "Region content:\n";
		for (SIZE_T i = 0; i < memInfo.RegionSize; ++i)
			cout << *((byte*)memInfo.BaseAddress + i) << " ";
}