#include <iostream>
#include <windows.h>
#include <fileapi.h>

#include "memoryhandler.h"

using std::cout;
using std::wcin;

void reserveRegion() {
	PVOID address = nullptr;
	SIZE_T regsize = 4096;
	wchar_t answer;

	// confirming action
	cout << "Do you really want to reserve a memory region? ('y' for 'yes'): ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) != 'Y')
		return;

	// get desired address if required
	cout << "Do you want to specify an address to reserve? ('y' for 'yes'): ";
	wcin >> answer;
	wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) == 'Y') {
		cout << "Enter the start of region you want to reserve (hex): ";
		wcin >> std::hex >> address >> std::dec;
		wcin.ignore(INT_MAX, '\n');

		cout << "Enter the size of region you want to reserve (dec): ";
		wcin >> regsize;
		wcin.ignore(INT_MAX, '\n');
	}
	else
		cout << "Address will be chosen automatically."
		<< "\nSize of region will be 4096 bytes.\n";

	// reserve a region
	address = VirtualAlloc(address, regsize, MEM_RESERVE, PAGE_READWRITE);

	// check if allocation was succsessful
	if (address)
		cout << "Allocation was succseffsul.\n"
			<< "\tStarting address is " << std::dec << address << ".\n";
	else {
		cout << "Allocation failed. Error " << GetLastError() << ".\n";
		SetLastError(0);
	}
}