#include <iostream>

#include <windows.h>

#include <omp.h.>

const unsigned requiredPrecision = 100000000;
const unsigned operationsPerBlock = 100;
const unsigned threadsNumber = 16;

int main() {
	double pi = 0;
	int blocksNumber = requiredPrecision / operationsPerBlock,
		startTime, endTime;
	omp_lock_t lock;

	omp_set_num_threads(threadsNumber);
	omp_init_lock(&lock);

	startTime = GetTickCount();

	#pragma omp parallel for schedule(dynamic, operationsPerBlock) reduction(+:pi)
		for (int i = 0; i < requiredPrecision; ++i)
			pi += 4 / (1 + ((i + 0.5) / requiredPrecision) * ((i + 0.5) / requiredPrecision));

	endTime = GetTickCount();

	pi /= requiredPrecision;

	std::cout.precision(requiredPrecision);
	std::cout << pi << "\nTime taken: " << endTime - startTime << ".";

	return 0;
}
