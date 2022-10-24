#pragma once

/*
		getSysInfo() prints the information about system:
	processors, memory range, etc
*/
void getSysInfo();

/*
		getMemStatus() prints the information about memory
*/
void getMemStatus();

/*
		memoryState() defines status of concrete memory area
	which address is defined by user 
*/
void memoryState();

/*
		reserveRegion() reserves a memory region
	which address can be defined automatically or by user
*/
void reserveRegion();

/*
		reserveRegionAndGiveMemory() reserves a memory region
	which address can be defined automatically or by user
	and gives it a sprcified amount of memory
*/
void reserveRegionAndGiveMemory();

/*
		writeData() fills memory areas which addresses
	are specified by user with concrete data
*/
void writeData();

/*
		protectRegion() sets protection to a 
	keyboard-specified memory region and checks it
*/
void protectRegion();

/*
		freeRegion() frees a keyboard-spcified
	memory region
*/
void freeRegion();