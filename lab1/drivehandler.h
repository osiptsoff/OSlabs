#pragma once

#include <iostream>

#include <windows.h>
#include <fileapi.h>

using std::cout;
using std::wcin;

/*
	printDriveInfo(LPTSTR) receives unicode path to drive root
	and prints info about the drive:
	type, name, serial number, max length of filename, file system name, volume
*/
void printDriveInfo(LPTSTR rootPath) {
	const DWORD volNameMaxLength = 50, fileSystemNameLength = 10;

	UINT driveType;
	wchar_t volName[volNameMaxLength], fileSystemName[fileSystemNameLength];
	DWORD sectorPerCluster, bytesPerSector, numOfFreeClusters, totalNumOfClusters, maxComponentLength, serialNumber, flags;

	// drive type
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

	// names
	GetVolumeInformationW(rootPath, volName, volNameMaxLength, &serialNumber, &maxComponentLength, &flags, fileSystemName, fileSystemNameLength);
	std::wcout << "Drive name: " << volName
		<< "\nSerial number: " << serialNumber
		<< "\nMax file name length: " << maxComponentLength
		<< "\nFile system: " << fileSystemName;

	// flags
	if (flags & FILE_CASE_SENSITIVE_SEARCH)
		cout << "\nThe specified volume supports case-sensitive file names.";
	if (flags & FILE_CASE_PRESERVED_NAMES)
		cout << "\nThe specified volume supports preserved case of file names when it places a name on disk.";
	if (flags & FILE_UNICODE_ON_DISK)
		cout << "\nThe specified volume supports Unicode in file names as they appear on disk.";
	if (flags & FILE_PERSISTENT_ACLS)
		cout << "\nThe specified volume preserves and enforces access control lists (ACL).";
	if (flags & FILE_FILE_COMPRESSION)
		cout << "\nThe specified volume supports file-based compression.";
	if (flags & FILE_VOLUME_QUOTAS)
		cout << "\nThe specified volume supports disk quotas.";
	if (flags & FILE_SUPPORTS_SPARSE_FILES)
		cout << "\nThe specified volume supports sparse files.";
	if (flags & FILE_SUPPORTS_REPARSE_POINTS)
		cout << "\nThe specified volume supports reparse points.";
	if (flags & FILE_SUPPORTS_REMOTE_STORAGE)
		cout << "\nThe specified volume supports remote storage.";
	if (flags & FILE_RETURNS_CLEANUP_RESULT_INFO)
		cout << "\nThe specified volume returns cleanup result info.";
	if (flags & FILE_SUPPORTS_POSIX_UNLINK_RENAME)
		cout << "\nThe specified volume supports posix unlink rename.";
	if (flags & FILE_VOLUME_IS_COMPRESSED)
		cout << "\nThe specified volume is a compressed volume, for example, a DoubleSpace volume.";
	if (flags & FILE_SUPPORTS_OBJECT_IDS)
		cout << "\nThe specified volume supports object identifiers.";
	if (flags & FILE_SUPPORTS_ENCRYPTION)
		cout << "\nThe specified volume supports the Encrypted File System (EFS).";
	if (flags & FILE_NAMED_STREAMS)
		cout << "\nThe specified volume supports named streams.";
	if (flags & FILE_READ_ONLY_VOLUME)
		cout << "\nThe specified volume is read-only.";
	if (flags & FILE_SEQUENTIAL_WRITE_ONCE)
		cout << "\nThe specified volume supports a single sequential write.";
	if (flags & FILE_SUPPORTS_TRANSACTIONS)
		cout << "\nThe specified volume supports transactions.";
	if (flags & FILE_SUPPORTS_HARD_LINKS)
		cout << "\nThe specified volume supports hard links.";
	if (flags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
		cout << "\nThe specified volume supports extended attributes.";
	if (flags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
		cout << "\nThe file system supports open by FileID.";
	if (flags & FILE_SUPPORTS_USN_JOURNAL)
		cout << "\nThe specified volume supports update sequence number (USN) journals.";
	if (flags & FILE_SUPPORTS_INTEGRITY_STREAMS)
		cout << "\nThe specified volume supports integrity streams.";
	if (flags & FILE_SUPPORTS_BLOCK_REFCOUNTING)
		cout << "\nThe specified volume supports sharing logical clusters between files on the same volume.";
	if (flags & FILE_SUPPORTS_SPARSE_VDL)
		cout << "\nThe specified volume supports sparse VDL.";
	if (flags & FILE_DAX_VOLUME)
		cout << "\nThe specified volume is a direct access (DAX) volume.";
	if (flags & FILE_SUPPORTS_GHOSTING)
		cout << "\nThe specified volume supports ghosting";

	// space
	GetDiskFreeSpace(rootPath, &sectorPerCluster, &bytesPerSector, &numOfFreeClusters, &totalNumOfClusters);
	cout << "\nTotal amount of clusters: " << totalNumOfClusters
		<< "\nAmount of free clusters: " << numOfFreeClusters
		<< "\nSectors per cluster: " << sectorPerCluster
		<< "\nBytes per sector: " << bytesPerSector << '\n' << std::endl;
}

/*
	printDrivesList() prints names of all of drives that can be found in this device
	and offers to get more specific info about them
*/
void printDrivesList() {
	int position = 0;
	DWORD drives = GetLogicalDrives();

	wchar_t choice, desiredDisk;
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
	wcin >> choice;
	wcin.ignore(INT_MAX, '\n');
	while (towupper(choice) == 'Y') {
		cout << "Type the drive you want to learn about ('D' for example): ";
		wcin >> desiredDisk;
		desiredDisk = towupper(desiredDisk);
		std::cin.ignore(INT_MAX, '\n');

		if (desiredDisk >= 'A' && desiredDisk <= 'Z' && drives & (1 << (desiredDisk - 'A'))) {
			path[0] = desiredDisk;
			printDriveInfo(path);
		} else cout << "Incorrect drive.\n";

		cout << "\nDo you want to get more specific info about one more of drives?\n"
			<< "Type 'y' for 'yes', any other key for 'no': ";
		wcin >> choice;
		wcin.ignore(INT_MAX, '\n');
	}
}