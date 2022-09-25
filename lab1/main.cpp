#include <iostream>

#include <windows.h>
#include <fileapi.h>

#include "drivehandler.h"
#include "filehandler.h"
#include "folderhandler.h"

using std::cout;
using std::cin;
using std::endl;

/*
    help() prints what this application can do
*/
void help() {
    cout << "Type 'a' to see all drives,\n"
        << "type 'b' to start creating new folder,\n"
        << "type 'c' to start removing folder,\n"
        << "type 'd' to start creating new file,\n"
        << "type 'e' to start moving file to another path,\n"
        << "type 'f' to get information of specified file attributes,\n"
        << "type 'g' to close this application.\n\n"
        << "Options must be typed without quotes.\n" << endl;
}


int main()
{
    char option = 'y';

    // array of all functions user can call from this level of interface
    void (*funcs[6])() = {
        printDrivesList,
        makeFolder,
        removeFolder,
        createNewFile,
        moveFile,
        getFileAttributes
    };
    
    while (option != 'g') {
        system("cls");
        help();
        
        cout << "Make your choice and press enter: ";
        cin >> option;
        std::cin.ignore(INT_MAX, '\n');

        if (option >= 'a' && option <= 'g') {
            if (option != 'g')
                funcs[option - 'a']();
        }
        else cout << "\nWrong choice.\n";

        system("pause");
    }

    cout << "\nProgram exited.\n";
    return 0;
}
