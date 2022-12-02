#include <iostream>

#include <windows.h>
#include <fileapi.h>
#include <synchapi.h>

const unsigned requiredPrecision = 100000000;
const unsigned operationsPerBlock = 100;
const unsigned threadsNumber = 16;

/*
	countPiByBlocks() counts operationsPerBlock digits of pi
	at every area with begin at k * threadsNumber * currentBlockStart

	takes void* pointer at array of parameters:
		piPart - output variable, address of calculated digits
		currentBlockStart - input variable, specifies location of blocks
*/ 
void countPiByBlocks(void *parametersStart) {
	// convert input variable to array of void* pointers in order to get params
	void** parameters = (void**)parametersStart;

	// get params from input array
	double* piPart = (double*)(parameters[0]);
	unsigned currentBlockStart = *(unsigned*)(parameters[1]);

	// while we are not put of pi bounds
	while (currentBlockStart < requiredPrecision) {
		// calculate block using given formula
		for (int i = currentBlockStart; i < currentBlockStart + operationsPerBlock; ++i) 
			*piPart += 4 / (1 + ((i + 0.5) / requiredPrecision) * ((i + 0.5) / requiredPrecision));
		
		// go to next block
		currentBlockStart += threadsNumber * operationsPerBlock;

	}

	*piPart /= requiredPrecision;
}

int main() {
	int startTime, endTime;
	
	// each thread takes an array of parameters
	//	(passed as void* to satisfy CreateThread() definition)

	HANDLE threads[threadsNumber];
	void* params[threadsNumber][2];
	double piParts[threadsNumber];
	int startingOperations[threadsNumber];

	double pi = 0;

	// assign input parameters, create threads
	// each thread calculates an isolated part of pi
	// in order to protect threads from race condition
	for (int i = 0; i < threadsNumber; ++i) {
		piParts[i] = 0;
		startingOperations[i] = i * operationsPerBlock;
		params[i][0] = piParts + i;
		params[i][1] = startingOperations + i;

		// threads are suspended because creating thread takes a time
		// at which previously created threads will perform,
		// reducing an accuracy of time measuring
		threads[i] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)countPiByBlocks,
			params[i],
			CREATE_SUSPENDED,
			NULL
		);
	}

	startTime = GetTickCount();
	for (int i = 0; i < threadsNumber; ++i)
		ResumeThread(threads[i]);

	WaitForMultipleObjects(threadsNumber, threads, TRUE, INFINITE);
	endTime = GetTickCount();

	// an answer is a sum of all parts calculated by threads
	for (int i = 0; i < threadsNumber; ++i)
		pi += piParts[i];

	std::cout.precision(requiredPrecision);
	std::cout << pi << "\nTime taken: " << endTime - startTime << ".";

	// close handles of threads
	for (int i = 0; i < threadsNumber; ++i)
		CloseHandle(threads[i]);

	return 0;
}