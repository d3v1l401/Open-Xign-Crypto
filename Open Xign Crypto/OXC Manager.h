#pragma once
#include <iostream>
#include <Windows.h>
#include "IDA.h"

#define LLD_ENABLE	1
#define	LLD_DISABLE	0
#define _LLDOPTION OXCManager::UseLowLevelDecryption

class __declspec(dllexport) OXCManager
{
private:
	
	static DWORD DW1;
	static DWORD DW2;
	static DWORD DW3;
	static int* EDIBUFF;
	static int* ESIBUFF;
	// Yep, there's an enabler here...defaultly it's on 1, so enabled.
	static DWORD UseLowLevelDecryption;

protected:



public:

	static unsigned int GetKey(PVOID lpMem, int Buffer);
	static void __cdecl CreateSBox(int EDIBuff, int ESIBuff, unsigned int Counter);
	static void *__cdecl Decrypt(void *OutBuff, const void *InBuff, unsigned int Size);
	static void *__cdecl DecryptWrap(void *OutBuff, const void *InBuff, unsigned int Size);
	static void SetDecryption(short int Option);
};