#include <iostream>

#include <windows.h>
#include <fileapi.h>
#include <synchapi.h>

const unsigned requiredPrecision = 100000000;
const unsigned operationsPerBlock = 100;
const unsigned threadsNumber = 16;

LPCRITICAL_SECTION csBlocksCountGrow = new CRITICAL_SECTION;
unsigned blocksDone = 0;

/*
	countPiByBlocks() counts pi by blocks of operationsPerBlock size
*/ 
void countPiByBlocks(void *parameter) {
	int currentBlockStart;
	double* piPart = (double*)parameter;

	EnterCriticalSection(csBlocksCountGrow);
	currentBlockStart = blocksDone++ * operationsPerBlock;
	LeaveCriticalSection(csBlocksCountGrow);

	// while we are not out of pi bounds
	while (currentBlockStart < requiredPrecision) {
		// calculate block using given formula
		for (int i = currentBlockStart; i < currentBlockStart + operationsPerBlock; ++i) 
			*piPart += 4 / (1 + ((i + 0.5) / requiredPrecision) * ((i + 0.5) / requiredPrecision));
		
		// go to next block
		EnterCriticalSection(csBlocksCountGrow);
		currentBlockStart = blocksDone++ * operationsPerBlock;
		LeaveCriticalSection(csBlocksCountGrow);
	}
}

int main() {
	int startTime, endTime;
	double pi = 0, piParts[threadsNumber];
	HANDLE threads[threadsNumber];

	InitializeCriticalSection(csBlocksCountGrow);

	// assign input parameters, create threads
	for (int i = 0; i < threadsNumber; ++i) {
		piParts[i] = 0;

		// threads are suspended because creating thread takes a time
		// at which previously created threads will perform,
		// reducing an accuracy of time measuring
		threads[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)countPiByBlocks,
			piParts + i,
			CREATE_SUSPENDED,
			NULL
		);
	}

	startTime = GetTickCount();
	for (int i = 0; i < threadsNumber; ++i)
		ResumeThread(threads[i]);

	WaitForMultipleObjects(threadsNumber, threads, TRUE, INFINITE);
	endTime = GetTickCount();

	for (int i = 0; i < threadsNumber; ++i)
		pi += piParts[i];
	pi /= requiredPrecision;

	std::cout.precision(requiredPrecision);
	std::cout << pi << "\nTime taken: " << endTime - startTime << ".";

	DeleteCriticalSection(csBlocksCountGrow);

	for (int i = 0; i < threadsNumber; ++i)
		CloseHandle(threads[i]);

	return 0;
}