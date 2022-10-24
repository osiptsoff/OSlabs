#include <iostream>
#include <windows.h>
#include <fileapi.h>

#include "memoryhandler.h"

using std::cout;

/*
	desypherOEM() prints the processor architecture abbreviation
	depending on given dword
*/
void desypherOEM(DWORD oem) {
	switch (oem) {
	case PROCESSOR_ARCHITECTURE_AMD64 :
		cout << "\nProcessor architecture: AMD64;\n";
		break;
	case PROCESSOR_ARCHITECTURE_ARM:
		cout << "\nProcessor architecture: ARM;\n";
		break;
	case PROCESSOR_ARCHITECTURE_ARM64:
		cout << "\nProcessor architecture: ARM64;\n";
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		cout << "\nProcessor architecture: Intel Itanium-based;\n";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		cout << "\nProcessor architecture: x86;\n";
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		cout << "\nProcessor architecture: unknown;\n";
		break;
	default:
		cout << "\nProcessor architecture: UNEXPECTED VALUE;\n";
	}
}

void getSysInfo() {
	LPSYSTEM_INFO sysInfo;
	wchar_t answer;

	// confirming action
	cout << "Do you really want to get information about system? ('y' for 'yes'): ";
	std::wcin >> answer;
	std::wcin.ignore(INT_MAX, '\n');
	if (towupper(answer) != 'Y')
		return;

	// getting system info
	sysInfo = new SYSTEM_INFO();
	GetNativeSystemInfo(sysInfo);

	// printing system info
	desypherOEM(sysInfo->wProcessorArchitecture);
	cout << "Size of page: " << sysInfo->dwPageSize << ";\n";
	cout << "Minimum address of memory allocation (hex): " << sysInfo->lpMinimumApplicationAddress << ";\n";
	cout << "Maximum address of memory allocation (hex): " << sysInfo->lpMaximumApplicationAddress << ";\n";
	cout << "Processors:\n";
	for (int i = 0; i < 32; ++i)
		if (sysInfo->dwActiveProcessorMask & (1 << i))
			cout << "\tProcessor " << i << " is configured into system\n";
		else
			cout << "\tProcessor " << i << " is not configured into system\n";
	cout << "Allocation granularity: " << sysInfo->dwAllocationGranularity << ";\n";
	cout << "Processor level: " << sysInfo->wProcessorLevel << ";\n";


	// freeing used memory
	delete sysInfo;
}