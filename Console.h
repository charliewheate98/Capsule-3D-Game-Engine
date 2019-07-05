#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>  
#include <sys/stat.h> 
#include <io.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;	// Get access to the std namespace

// Open the console for output
static void OpenConsole()
{
	AllocConsole();		// Allocate the console
	HANDLE stdHandle;	// Create a handle for the std
	int hConsole;	// Create a console handle
	FILE* fp;	// Create file for allocating console
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);	// Assign console handle
	hConsole = _open_osfhandle((long)stdHandle, _O_TEXT);	// Open output functionality using the console handle
	fp = _fdopen(hConsole, "w");	// Open the console

	freopen_s(&fp, "CONOUT$", "w", stdout);		// Open console with output functionality
}