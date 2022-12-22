#include <iostream>

#include <windows.h>
#include <fileapi.h>
#include <synchapi.h>

const unsigned requiredPrecision = 100000000;
const unsigned operationsPerBlock = 100;
const unsigned threadsNumber = 4;

/*
	countPiByBlocks() counts operationsPerBlock digits of pi
	at every area with begin at k * threadsNumber * currentBlockStart

	takes void* pointer at array of parameters:
		pi - output variable, address of variable where pi is stored
		currentBlockStart - input variable, specifies location of blocks
		mutex - mutex to synchronise on
*/ 
void countPiByBlocks(void *parametersStart) {
	// convert input variable to array of void* pointers in order to get params
	void** parameters = (void**)parametersStart;

	// get params from input array
	double* pi = (double*)(parameters[0]);
	unsigned currentBlockStart = *(unsigned*)(parameters[1]);
	HANDLE mutex = *(HANDLE*)(parameters[2]);

	double accumulator = 0;
	// while we are not put of pi bounds
	while (currentBlockStart < requiredPrecision) {
		// calculate block using given formula
		for (int i = currentBlockStart; i < currentBlockStart + operationsPerBlock; ++i) 
			accumulator += 4 / (1 + ((i + 0.5) / requiredPrecision) * ((i + 0.5) / requiredPrecision));
		
		// go to next block
		currentBlockStart += threadsNumber * operationsPerBlock;
	}

	WaitForSingleObject(mutex, INFINITE);
	*pi += accumulator / requiredPrecision;
	ReleaseMutex(mutex);
}

int main() {
	int startTime, endTime;
	
	// each thread takes an array of parameters
	//	(passed as void* to satisfy CreateThread() definition)

	HANDLE threads[threadsNumber], mutex;
	void* params[threadsNumber][3];
	int startingOperations[threadsNumber];

	mutex = CreateMutex(NULL, FALSE, NULL);
	double pi = 0;

	// assign input parameters, create threads
	for (int i = 0; i < threadsNumber; ++i) {
		startingOperations[i] = i * operationsPerBlock;
		params[i][0] = &pi;
		params[i][1] = startingOperations + i;
		params[i][2] = &mutex;

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


	std::cout.precision(requiredPrecision);
	std::cout << pi << "\nTime taken: " << endTime - startTime << ".";

	// close handles
	for (int i = 0; i < threadsNumber; ++i)
		CloseHandle(threads[i]);
	CloseHandle(mutex);

	return 0;
}