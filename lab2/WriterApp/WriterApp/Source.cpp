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
void help() {
    cout << "Type 'a' to create new file,\n"
        << "type 'b' to create a mapping object for created file,\n"
        << "type 'c' to create mapping for created mapping object,\n"
        << "type 'd' to unmap a map created previously,\n"
        << "type 'e' to write data to map,\n"
        << "type 'f' to close this application,\n"
        << "Options must be typed without quotes.\n" << std::endl;
}


int main()
{
    HANDLE file = INVALID_HANDLE_VALUE, fileMappingObject = NULL;
    LPVOID mapspace = NULL;

    size fileSize, offset;
    SIZE_T mappingSize, sizeToWrite;

    wchar_t option = 'y';
    wchar_t path[MAX_PATH], fileMappingObjectName[MAX_PATH];

    while (option != 'F') {
        system("cls");
        help();

        cout << "Make your choice and press enter: ";
        wcin >> option;
        option = toupper(option);
        std::cin.ignore(INT_MAX, '\n');

        switch (option) {
            // create new file
        case 'A':
            cout << "Enter the name of file, max 255 symbols (D:\\Users\\Me\\file.txt for example),"
                << "\nprevious file (if opened) will be closed: ";

            CloseHandle(file);

            wcin.getline(path, MAX_PATH, L'\n');
            file = CreateFile(path,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ,
                NULL,
                CREATE_NEW,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );

            if (file == INVALID_HANDLE_VALUE) {
                cout << "Failed to create. Error: " << GetLastError() << ".\n";
                SetLastError(0);
            }
            else
                cout << "File created succsessfully.\n ";

            break;

            // make a mapping object
        case 'B':
            cout << "A mapping will be created for previously opened file.\n"
                << "Enter the name which will be used by reader app to open a map (Local\\Name for example): ";
            wcin.getline(fileMappingObjectName, MAX_PATH, L'\n');
            cout << "Enter the maximum size of space you will map: ";
            wcin >> fileSize.total;

            CloseHandle(fileMappingObject);

            fileMappingObject = CreateFileMapping(file,
                NULL,
                PAGE_READWRITE,
                fileSize.higher,
                fileSize.lesser,
                fileMappingObjectName
            );

            if (fileMappingObject == NULL) {
                cout << "Failed to create. Error: " << GetLastError() << ".\n";
                SetLastError(0);
            }
            else
                cout << "Mapping object created succsessfully.\n ";

            break;

            // make a map
        case 'C':
            cout << "Enter the byte from which file will be mapped: ";
            wcin >> offset.total;

            cout << "Enter the number of bytes to be mapped: ";
            wcin >> mappingSize;

            UnmapViewOfFile(mapspace);

            mapspace = MapViewOfFile(fileMappingObject,
                FILE_MAP_WRITE,
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
        case 'D':
            if (mapspace == NULL)
                cout << "You had nothing to unmap.\n";
            else
                cout << "Unmapped.\n ";

            UnmapViewOfFile(mapspace);
            mapspace = NULL;

            break;

            // write data
        case 'E':
            if (mapspace == NULL) {
                cout << "You have nowhere to write.\n";
                break;

            }

            cout << "Specify how many bytes do you want to write: ";
            wcin >> sizeToWrite;

            if (sizeToWrite > mappingSize) {
                cout << "Desired size exceeds the size of map.\n";
                break;
            }

            byte* data = (byte*)mapspace + offset.total;

            cout << "Enter bytes (0 to 255), one by one: ";
            unsigned numericView;
            for (SIZE_T i = 0; i < sizeToWrite; ++i) {
                std::cin >> numericView;
                *(data++) = numericView;
            }

            cout << "Done.\n";
        }

        system("pause");
    }

    cout << "\nProgram exited.\n";
    return 0;
}
