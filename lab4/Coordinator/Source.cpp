#include <iostream>
#include <windows.h>

int main() {
	const unsigned pageSizeBytes = 4096,
				numOfPages = 9;
	const wchar_t readerPath[] = L"C:\\Users\\Nikita\\source\\repos\\OSlabs\\Debug\\ReaderApp.exe",
				writerPath[] = L"C:\\Users\\Nikita\\source\\repos\\OSlabs\\Debug\\WriterApp.exe";
	
	HANDLE readingSemaphores[numOfPages],
		writingSemaphores[numOfPages],
		processHandles[numOfPages * 2],
		sharedFile,
		sharedFileMappingObject;
	PROCESS_INFORMATION readerInfo[numOfPages],
		writerInfo[numOfPages];
	STARTUPINFO startupInfo;
	SECURITY_ATTRIBUTES semaphoreSecurityAttributes = { sizeof(semaphoreSecurityAttributes), NULL, TRUE};
	wchar_t dummyBuffer[50] = L"",
		readingSemaphoreName[numOfPages][15],
		writingSemaphoreName[numOfPages][15];

	sharedFile = CreateFile(L"sharedfile.txt",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	sharedFileMappingObject = CreateFileMapping(sharedFile,
		NULL,
		PAGE_READWRITE,
		0,
		pageSizeBytes * numOfPages,
		L"Local\\SharedFile"
	);
	
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	for (int i = 0; i < numOfPages; ++i) {
		wcscpy_s(readingSemaphoreName[i], L"Local\\rSemX");
		readingSemaphoreName[i][10] = '0' + i;
		wcscpy_s(writingSemaphoreName[i], L"Local\\wSemX");
		writingSemaphoreName[i][10] = '0' + i;

		readingSemaphores[i] = CreateSemaphore(&semaphoreSecurityAttributes, 0, 1, readingSemaphoreName[i]);
		writingSemaphores[i] = CreateSemaphore(&semaphoreSecurityAttributes, 1, 1, writingSemaphoreName[i]);

		ZeroMemory(readerInfo + i, sizeof(*readerInfo));
		ZeroMemory(writerInfo + i, sizeof(*writerInfo));
	}
	for(int i = 0; i < numOfPages; ++i) {
		CreateProcess(readerPath,
			dummyBuffer,
			NULL,
			NULL,
			TRUE,
			CREATE_SUSPENDED,
			NULL,
			NULL,
			&startupInfo,
			readerInfo + i
		);
		CreateProcess(writerPath,
			dummyBuffer,
			NULL,
			NULL,
			TRUE,
			CREATE_SUSPENDED,
			NULL,
			NULL,
			&startupInfo,
			writerInfo + i
		);

		processHandles[i] = (readerInfo + i)->hProcess;
		processHandles[numOfPages * 2 - 1 - i] = (writerInfo + i)->hProcess;
	}

	for (int i = 0; i < numOfPages; ++i) {
		ResumeThread((readerInfo + i)->hThread);
		ResumeThread((writerInfo + i)->hThread);
	}
	WaitForMultipleObjects(numOfPages * 2, processHandles, TRUE, INFINITE);

	for (int i = 0; i < numOfPages; ++i) {
		CloseHandle((readerInfo + i)->hProcess);
		CloseHandle((readerInfo + i)->hThread);

		CloseHandle((writerInfo + i)->hProcess);
		CloseHandle((writerInfo + i)->hThread);
		
		CloseHandle(readingSemaphores[i]);
		CloseHandle(writingSemaphores[i]);
	}
	CloseHandle(sharedFile);
	CloseHandle(sharedFileMappingObject);

	return 0;
}