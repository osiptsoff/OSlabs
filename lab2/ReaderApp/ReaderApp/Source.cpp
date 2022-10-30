#include <iostream>
#include <windows.h>
#include <fileapi.h>

using std::cout;
using std::wcin;

typedef union {
    unsigned long long total;
    struct {
        DWORD lesser;
        DWORD higher;
    };
} size;

/*
    help() prints what this application can do
*/
//CHANGE
void help() {
    cout << "Type 'a' to open a file mapping object,\n"
        << "type 'b' to create mapping,\n"
        << "type 'c' to unmap a map created previously,\n"
        << "type 'd' to show mapped data,\n"
        << "type 'e' to close this application,\n"
        << "Options must be typed without quotes.\n" << std::endl;
}


int main()
{
    HANDLE fileMappingObject = NULL;
    LPVOID mapspace = NULL;

    size offset;
    SIZE_T mappingSize;

    wchar_t option = 'y';
    wchar_t fileMappingObjectName[MAX_PATH];

    while (option != 'E') {
        system("cls");
        help();

        cout << "Make your choice and press enter: ";
        wcin >> option;
        option = toupper(option);
        std::cin.ignore(INT_MAX, '\n');

        switch (option) {
            // open file mapping object
        case 'A':
            cout << "Enter the name of file mapping object, max 255 symbols (Local\\Name for example),"
                << "\nprevious file (if opened) will be closed: ";

            CloseHandle(fileMappingObject);

            wcin.getline(fileMappingObjectName, MAX_PATH, L'\n');
            fileMappingObject = OpenFileMapping(FILE_MAP_READ,
                FALSE,
                fileMappingObjectName
            );

            if (fileMappingObject == NULL) {
                cout << "Failed to open. Error: " << GetLastError() << ".\n";
                SetLastError(0);
            }
            else
                cout << "File mapping object opened succsessfully.\n ";

            break;

            // make a map
        case 'B':
            cout << "Enter the byte from which file will be mapped: ";
            wcin >> offset.total;

            cout << "Enter the number of bytes to be mapped: ";
            wcin >> mappingSize;

            UnmapViewOfFile(mapspace);

            mapspace = MapViewOfFile(fileMappingObject,
                FILE_MAP_READ,
                0,
                0,
                mappingSize
            );

            if (mapspace == NULL) {
                cout << "Failed to create a map. Error: " << GetLastError() << ".\n";
                SetLastError(0);
            }
            else
                cout << "Map created succsessfully.\n ";

            break;

            // unmap
        case 'C':
            if (mapspace == NULL)
                cout << "You had nothing to unmap.\n";
            else
                cout << "Unmapped.\n ";

            UnmapViewOfFile(mapspace);
            mapspace = NULL;

            break;

            // read data
        case 'D':
            if (mapspace == NULL) {
                cout << "You have nowhere to read from.\n";
                break;
            }

            byte* data = (byte*)mapspace + offset.total;

            cout << "Enter bytes (0 to 255), one by one: ";
            unsigned numericView;
            for (SIZE_T i = 0; i < mappingSize; ++i) {
                numericView = *(data++);
                cout << numericView << " ";
            }

            cout << "Done.\n";
        }

        system("pause");
    }

    cout << "\nProgram exited.\n";
    return 0;
}
