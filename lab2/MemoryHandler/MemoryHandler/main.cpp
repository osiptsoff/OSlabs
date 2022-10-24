#include <iostream>

#include "memoryhandler.h"

using std::cout;
using std::cin;
using std::endl;

/*
    help() prints what this application can do
*/
void help() {
    cout << "Type 'a' to get info about system,\n"
        << "type 'b' to get info about memory,\n"
        << "type 'c' to define state of a concrete memory area,\n"
        << "type 'd' to reserve a memory region,\n"
        << "type 'e' to reserve a memory region and give it a memory,\n"
        << "type 'f' to write data to memory area,\n"
        << "type 'g' to protect a concrete memory region\n"
        << "type 'h' to free a concrete memory region,\n"
        << "type 'i' to close this application.\n\n"
        << "Options must be typed without quotes.\n" << endl;
}


int main()
{
    char option = 'y';

    // array of all functions user can call from this level of interface
    void (*funcs[])() = {
        getSysInfo,
        getMemStatus,
        memoryState,
        reserveRegion,
        reserveRegionAndGiveMemory,
        writeData,
        protectRegion,
        freeRegion
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
