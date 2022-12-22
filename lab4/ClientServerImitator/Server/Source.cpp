#include <iostream>
#include <windows.h>

using std::cout;
using std::wcin;
using std::endl;

void help() {
    cout << "Type 'a' to create a named pipe,\n"
        << "type 'b' to connect named pipe,\n"
        << "type 'c' to disconnect named pipe,\n"
        << "type 'd' to write to pipe,\n"
        << "type 'e' to wait for writing's end,\n"
        << "type 'f' to close this application.\n"
        << "Options must be typed without quotes.\n" << std::endl;
}

void _stdcall AfterWriteRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
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

    unsigned messageLength, error, waitResult;

    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    while (option != 'F') {
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

            pipe = CreateNamedPipe(pipeName,
                PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
                PIPE_TYPE_MESSAGE,
                2,
                MAX_PATH,
                MAX_PATH,
                0,
                NULL
            );

            if (pipe == INVALID_HANDLE_VALUE) {
                cout << "Error " << GetLastError() << ".\n";
                SetLastError(0);
            } else 

                cout << "Pipe created.\n";
            break;
        case 'B':
            cout << "You will try to connect to pipe.\n";

            if(ConnectNamedPipe(pipe, &pipeOverlapped))
                cout << "Connection established.\n";
            else {
                error = GetLastError();

                if (error == ERROR_IO_PENDING) {
                    cout << "Waiting for connection.\n";
                    
                    while(HasOverlappedIoCompleted(&pipeOverlapped));

                    cout << "Connected.\n";

                    ResetEvent(hEvent);
                } else
                    cout << "Error " << error << ".\n";

                SetLastError(0);
            }

            break;
        case 'C':
            cout << "You will try to break connection with pipe.\n";

            if(DisconnectNamedPipe(pipe))
                cout << "Connection broken succsessfully.\n";
            else {
                cout << "Error " << GetLastError() << ".\n";
                SetLastError(0);
            }

            break;

        case 'D':
            cout << "You will write to pipe. Enter your message: ";
            std::cin.getline(message, MAX_PATH, L'\n');;
            messageLength = strlen(message);

            ZeroMemory(&pipeOverlapped, sizeof(pipeOverlapped));
            pipeOverlapped.hEvent = hEvent;

            if (WriteFileEx(pipe, message, messageLength, &pipeOverlapped, AfterWriteRoutine))
                cout << "Asynchronous writing started, you may wait for it through menu.\n";
            else {
                cout << "Error " << GetLastError() << ".\n";
                SetLastError(0);
            }

            break;
        case 'E':
            cout << "You will wait for asynchronous I/O to end.\n";

            SleepEx(0, TRUE);

            waitResult = WaitForSingleObject(hEvent, 2000);
            if (waitResult == WAIT_TIMEOUT)
                cout << "Waiting time exceeded. Try one more if there is an I/O operation pending.\n";
            else if (waitResult == WAIT_OBJECT_0) 
                cout << "Waiting is over.\n";

            ResetEvent(hEvent);
        }

        system("pause");
    }

    CloseHandle(hEvent);
    CloseHandle(pipe);

    cout << "\nProgram exited.\n";
    return 0;
}