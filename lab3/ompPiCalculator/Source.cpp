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

	// outer for-loop will be performed by threadsNumber threads,
	// so they will execute calculations by blocks with operationsPerBlock operations in each one;
	// every thread has increment variable where result of its calculations is accumulated,
	// this variable then will be added to total result (pi variable) in synchronized section
	#pragma omp parallel
	{
		double increment = 0;

		#pragma omp for
		for (int block = 0; block < blocksNumber; ++block)
			for (int i = block * operationsPerBlock; i < block * operationsPerBlock + operationsPerBlock; ++i)
				increment += 4 / (1 + ((i + 0.5) / requiredPrecision) * ((i + 0.5) / requiredPrecision));

		omp_set_lock(&lock);
		pi += increment;
		omp_unset_lock(&lock);
	}
	endTime = GetTickCount();

	pi /= requiredPrecision;

	std::cout.precision(requiredPrecision);
	std::cout << pi << "\nTime taken: " << endTime - startTime << ".";

	return 0;
}
