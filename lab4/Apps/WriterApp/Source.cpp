#include <fstream>
#include <windows.h>

void log(const char* message, int pageNo, std::ostream& logfile) {
	logfile << message << " at " << GetTickCount64();
	if(pageNo > 0)
		logfile << ", page number " << pageNo;
	logfile << "\n";
}

int main() {
	const unsigned pageSizeBytes = 4096, 
		numOfPages = 9,
		actionsCount = 10;

	HANDLE readingSemaphores[numOfPages],
		writingSemaphores[numOfPages],
		sharedFileMappingObject;
	std::ofstream logfile;
	void *pages[numOfPages],
		*pagesGlobal;;
	wchar_t readingSemaphoreName[numOfPages][15],
		writingSemaphoreName[numOfPages][15],
		logfileName[50] = L"log\\writers\\log_",
		processIdString[10];
	int waitTimeMs, freePageIndex, semaphoreAcquisitionResult;

	sharedFileMappingObject = OpenFileMapping(FILE_MAP_WRITE, FALSE, L"Local\\SharedFile");

	_itow_s(GetCurrentProcessId(), processIdString, 10);
	wcscat_s(logfileName, processIdString);
	wcscat_s(logfileName, L".txt");
	logfile.open(logfileName, std::ofstream::out);

	pagesGlobal = MapViewOfFile(sharedFileMappingObject, FILE_MAP_WRITE, 0, 0, numOfPages * pageSizeBytes);
	VirtualLock(pagesGlobal, numOfPages * pageSizeBytes);

	for (int i = 0; i < numOfPages; ++i) {
		wcscpy_s(readingSemaphoreName[i], L"Local\\rSemX");
		readingSemaphoreName[i][10] = '0' + i;
		wcscpy_s(writingSemaphoreName[i], L"Local\\wSemX");
		writingSemaphoreName[i][10] = '0' + i;

		readingSemaphores[i] = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, readingSemaphoreName[i]);
		writingSemaphores[i] = OpenSemaphore(SYNCHRONIZE, FALSE, writingSemaphoreName[i]);

		pages[i] = (byte*)pagesGlobal + i * pageSizeBytes;
	}

	for (int i = 0; i < actionsCount; ++i) {
		log("Started waiting", -1, logfile);
		freePageIndex = WaitForMultipleObjects(numOfPages, writingSemaphores, FALSE, INFINITE) - WAIT_OBJECT_0;
		log("Acquired", freePageIndex, logfile);

		if (freePageIndex >= 0 && freePageIndex < numOfPages) {
			waitTimeMs = rand() % 1001 + 500;
			log("Started reading", freePageIndex, logfile);
			Sleep(waitTimeMs);
			log("Ended reading", freePageIndex, logfile);
			ReleaseSemaphore(readingSemaphores[freePageIndex], 1, NULL);
			log("Released", freePageIndex, logfile);
		}
	}

	for (int i = 0; i < numOfPages; ++i) {
		CloseHandle(readingSemaphores[i]);
		CloseHandle(writingSemaphores[i]);
	}
	VirtualUnlock(pagesGlobal, pageSizeBytes);
	UnmapViewOfFile(pagesGlobal);
	logfile.close();

	return 0;
}