#include <iostream>
#include <windows.h>
#include <fileapi.h>

#include "memoryhandler.h"

using std::cout;
using std::wcin;

void writeData() {
	MEMORY_BASIC_INFORMATION memInfo;
	PVOID address = nullptr;
	wchar_t answer;

	// confirming action
	cout << "Do you really want to fill any memory region? ('y' for 'yes'): ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) != 'Y')
		return;

	// specify region
	cout << "Enter the address belonging to region you want to fill (hex): ";
	wcin >> std::hex >> address >> std::dec;
	wcin.ignore(INT_MAX, '\n');
	if (address == nullptr) {
		cout << "Failed to parse address.\n";
		return;
	}

	// get info about region
	VirtualQuery(address, &memInfo, sizeof(memInfo));

	// check for mistakes at getting info
	if (GetLastError() != 0) {
		cout << "Failed. Error " << GetLastError() << ".\n";
		SetLastError(0);
		return;
	}

	// check if memory is accessable
	if (memInfo.State != MEM_COMMIT) {
		cout << "Failed. Memory is not allocated.\n";
		return;
	}

	// define max size and desired size of written data
	SIZE_T maxSize = memInfo.RegionSize - ((SIZE_T)address - (SIZE_T)memInfo.BaseAddress);
	SIZE_T desiredSize = 0;
	cout << "You can write to this region. You can write max " << maxSize << " bytes.\n"
		<< "How many bytes do you need to write? ";
	wcin >> desiredSize;
	wcin.ignore(INT_MAX, '\n');

	// check if input was incorrect
	if (desiredSize == 0 || desiredSize > maxSize) {
		cout << "You cannot write this amount of data: " << desiredSize << " bytes. Operation aborted.\n";
		return;
	}

	// write data
	unsigned numericView;
	byte* data = (byte*)address;
	cout << "Type " << desiredSize << " bytes (0 to 255), one by one: ";
	for (SIZE_T i = 0; i < desiredSize; ++i) {
		std::cin >> numericView;
		*(data++) = numericView;
	}

	// show typed info
	cout << "\nYou have entered: ";
	for (SIZE_T i = 0; i < desiredSize; ++i) {
		numericView = *((byte*)address + i);
		cout << numericView << " ";
	}
}