#include <iostream>

#include "drivehandler.h"
#include "filehandler.h"
#include "folderhandler.h"
#include "asynchcopy.h"
#include "filepropshandler.h"

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
        << "type 'f' to start copying file to another path,\n"
        << "type 'g' to start copying file asyncronously (1.2)\n"
        << "type 'h' to get information of specified file attributes and change them if required,\n"
        << "type 'i' to close this application.\n\n"
        << "Options must be typed without quotes.\n" << endl;
}


int main()
{
    char option = 'y';

    // array of all functions user can call from this level of interface
    void (*funcs[])() = {
        printDrivesList,
        makeFolder,
        removeFolder,
        createNewFile,
        moveFile,
        copyFile,
        copyFileAsynch,
        processFileAttributes
    };
    
    while (option != 'I') {
        system("cls");
        help();
        
        cout << "Make your choice and press enter: ";
        cin >> option;
        option = toupper(option);
        std::cin.ignore(INT_MAX, '\n');

        if (option >= 'A' && option <= 'I') {
            if (option != 'I')
                funcs[option - 'A']();
        }
        else cout << "\nWrong choice.\n";

        system("pause");
    }

    cout << "\nProgram exited.\n";
    return 0;
}
