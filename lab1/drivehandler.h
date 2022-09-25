#pragma once

#include <iostream>

#include <windows.h>
#include <fileapi.h>

using std::cout;
using std::cin;

/*
	printDriveInfo(LPTSTR) receives unicode path to drive root
	and prints info about the drive:
	type, name, serial number, max length of filename, file system name, volume
*/
void printDriveInfo(LPTSTR rootPath) {
	const DWORD volNameMaxLength = 50, fileSystemNameLength = 10;

	UINT driveType;
	wchar_t volName[volNameMaxLength], fileSystemName[fileSystemNameLength];
	DWORD sectorPerCluster, bytesPerSector, numOfFreeClusters, totalNumOfClusters, maxComponentLength, serialNumber;

	driveType = GetDriveType(rootPath);
	switch (driveType) {
	case 0 :
		cout << "\nUndefinable\n";
		break;
	case DRIVE_REMOVABLE:
		cout << "\nRemovable disk\n";
		break;
	case DRIVE_FIXED:
		cout << "\nFixed disk\n";
		break;
	case DRIVE_REMOTE:
		cout << "\nRemote disk\n";
		break;
	case DRIVE_CDROM:
		cout << "\nCD-ROM\n";
		break;
	case DRIVE_RAMDISK:
		cout << "\nRAM-disk\n";
		break;
	default:
		cout << "\nSomething unexpected...\n";
	}

	GetVolumeInformationW(rootPath, volName, volNameMaxLength, &serialNumber, &maxComponentLength, NULL, fileSystemName, fileSystemNameLength);
	std::wcout << "Drive name: " << volName
		<< "\nSerial number: " << serialNumber
		<< "\nMax file name length: " << maxComponentLength
		<< "\nFile system: " << fileSystemName;


	GetDiskFreeSpace(rootPath, &sectorPerCluster, &bytesPerSector, &numOfFreeClusters, &totalNumOfClusters);
	cout << "\nTotal amount of clusters: " << totalNumOfClusters
		<< "\nAmount of free clusters: " << numOfFreeClusters
		<< "\nSectors per cluster: " << sectorPerCluster
		<< "\nBytes per sector: " << bytesPerSector << '\n' << std::endl;
}

/*
	printDrivesList() prints names all of drives that can be found in this device
	and offers to get more specific info about one of them
*/
void printDrivesList() {
	int position = 0;
	DWORD drives = GetLogicalDrives();

	char choice, desiredDisk;
	wchar_t path[] = L"A:\\";

	// get all drive names from DWORD drives by bit shifting
	cout << "Present drives: \n\n";
	for (DWORD i = 1; position < sizeof(DWORD) * 8; i <<= 1, ++position) {
		if (drives & i)
			cout << "Drive " << char('A' + position) << ":\n";
	}
	cout << "\nAbove are all drives found.\n";
	

	cout << "\nDo you want to get more specific info about one of drives?\n"
		 << "Type 'y' for 'yes', any other key for 'no': ";

	// print info about drive if required
	cin >> choice;
	std::cin.ignore(INT_MAX, '\n');
	while (choice == 'y') {
		cout << "Type the drive you want to learn about ('D' for example): ";
		cin >> desiredDisk;
		std::cin.ignore(INT_MAX, '\n');

		if (desiredDisk >= 'A' && desiredDisk <= 'Z' && drives & (1 << (desiredDisk - 'A'))) {
			path[0] = desiredDisk;
			printDriveInfo(path);
		} else cout << "Incorrect drive.\n";

		cout << "\nDo you want to get more specific info about one more of drives?\n"
			<< "Type 'y' for 'yes', any other key for 'no': ";
		cin >> choice;
		std::cin.ignore(INT_MAX, '\n');
	}
}