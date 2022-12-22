#include <iostream>
#include <windows.h>

using std::cout;
using std::wcin;
using std::endl;

void help() {
    cout << "Type 'a' to connect named pipe,\n"
        << "type 'b' to start reading from pipe\n"
        << "type 'c' to show the message,\n"
        << "type 'd' to close this application.\n"
        << "Options must be typed without quotes.\n" << std::endl;
}

void _stdcall AfterReadRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0)
        cout << "Error " << dwErrorCode << " in completion routine.";

    SetEvent(lpOverlapped->hEvent);
}

int main() {
    HANDLE hEvent = NULL;
    HANDLE pipe = NULL;
    OVERLAPPED pipeOverlapped;

    ZeroMemory(&pipeOverlapped, sizeof(pipeOverlapped));
    pipeOverlapped.hEvent = hEvent;

    wchar_t option = 'y';
    wchar_t pipeName[MAX_PATH];
    char message[MAX_PATH - 5];

    unsigned messageLength = 0, error, waitResult;

    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    while (option != 'D') {
        system("cls");
        help();

        cout << "Make your choice and press enter: ";
        wcin >> option;
        option = toupper(option);
        std::cin.ignore(INT_MAX, '\n');

        switch (option) {
        case 'A':
            cout << "Enter pipe name (like '\\\\.\\pipe\\pipename', no quotes): ";
            wcin.getline(pipeName, MAX_PATH, L'\n');;

            CloseHandle(pipe);

            pipe = CreateFile(pipeName,
                GENERIC_READ,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
                NULL
            );

            if (pipe == INVALID_HANDLE_VALUE) {
                cout << "Error " << GetLastError() << ".\n";
                SetLastError(0);
            }
            else

                cout << "Connection is succsessfull.\n";
            break;

        case 'B':
            cout << "You will start reading from pipe.\n ";
            cout << "Enter how many bytes you want to read: ";
            wcin >> messageLength;
            std::cin.ignore(INT_MAX, '\n');

            ZeroMemory(&pipeOverlapped, sizeof(pipeOverlapped));
            pipeOverlapped.hEvent = hEvent;

            if (ReadFileEx(pipe, message, messageLength, &pipeOverlapped, AfterReadRoutine))
                cout << "Asynchronous reading started, you may wait for it through menu.\n";
            else
                cout << "Error " << GetLastError() << ".\n";

            SetLastError(0);

            break;
        case 'C':
            cout << "You will wait for asynchronous I/O to end.\n";

            SleepEx(0, TRUE);

            waitResult = WaitForSingleObject(hEvent, 2000);
            if (waitResult == WAIT_TIMEOUT)
                cout << "Waiting time exceeded. Try one more if there is an I/O operation pending.\n";
            else if (waitResult == WAIT_OBJECT_0) {
                message[messageLength] = '\0';
                std::wcout << "Waiting is over. Message: " << message << "\n";
            }

            ResetEvent(hEvent);
        }

        system("pause");
    }

    CloseHandle(hEvent);
    CloseHandle(pipe);

    cout << "\nProgram exited.\n";
    return 0;
}