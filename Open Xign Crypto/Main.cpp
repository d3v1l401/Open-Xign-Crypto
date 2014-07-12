#include <iostream>
#include <Windows.h>

BOOL WINAPI DllMain(HMODULE ModuleHandle, INT Reason, PVOID Reserved)
{
	printf("\n\t[Open Xign Crypto]\nMade by d3v1l401 (http://d3vsite.org/)\nGPLv3 Licensed\nBased on Xign Code 3\n\n");
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:

		break;

	case DLL_THREAD_ATTACH:

		break;
	}
	return TRUE;
}