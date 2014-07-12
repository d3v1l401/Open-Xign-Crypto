#include "OXC Manager.h"

DWORD	OXCManager::DW1 = NULL;
DWORD	OXCManager::DW2 = NULL;
DWORD	OXCManager::DW3 = NULL;
int*	OXCManager::EDIBUFF = nullptr;
int*	OXCManager::ESIBUFF = nullptr;
DWORD	OXCManager::UseLowLevelDecryption = 1;

// LpMem: Heap address of the buff
unsigned int OXCManager::GetKey(PVOID lpMem, int Buffer)
{
	unsigned int result = NULL;

	for (result = (unsigned int)lpMem; result < (unsigned int)((char *)lpMem + 20 * OXCManager::DW3); result += 20)
	{
		if ((unsigned int)(Buffer - *(_DWORD *)(result + 12)) < 0x100000)
			return result;
	}
	return 0;
}

// EDIBuff and ESIBuff must be mallocated.
void __cdecl OXCManager::CreateSBox(int EDIBuff, int ESIBuff, unsigned int Counter)
{
	unsigned int CountLeft; // ecx@1
	int _EDI; // edi@1
	int _ESI; // esi@1

	/*
	__asm
	{
		mov edi, dword ptr[EDIBuff]
		mov esi, dword ptr[ESIBuff]
	}
	*/
	_ESI = ESIBuff;
	_EDI = EDIBuff;
	CountLeft = Counter >> 7;
	do
	{
		__asm
		{
			movdqa  xmm0, xmmword ptr[_ESI]
			movdqa  xmm1, xmmword ptr[_ESI + 10h]
			movdqa  xmm2, xmmword ptr[_ESI + 20h]
			movdqa  xmm3, xmmword ptr[_ESI+ 30h]
			movdqa  xmmword ptr[_EDI], xmm0
			movdqa  xmmword ptr[_EDI + 10h], xmm1
			movdqa  xmmword ptr[_EDI + 20h], xmm2
			movdqa  xmmword ptr[_EDI + 30h], xmm3
			movdqa  xmm4, xmmword ptr[_ESI + 40h]
			movdqa  xmm5, xmmword ptr[_ESI + 50h]
			movdqa  xmm6, xmmword ptr[_ESI + 60h]
			movdqa  xmm7, xmmword ptr[_ESI + 70h]
			movdqa  xmmword ptr[_EDI + 40h], xmm4
			movdqa  xmmword ptr[_EDI + 50h], xmm5
			movdqa  xmmword ptr[_EDI + 60h], xmm6
			movdqa  xmmword ptr[_EDI + 70h], xmm7
		}
		_ESI += 128;
		_EDI += 128;
		--CountLeft;
	} while (CountLeft);
	OXCManager::EDIBUFF = (int*)EDIBuff;
	OXCManager::ESIBUFF = (int*)ESIBuff;
}

void *__cdecl OXCManager::Decrypt(void *OutBuff, const void *InBuff, unsigned int Size)
{
	void *result; // eax@1
	int v4; // ecx@1
	unsigned int v5; // ecx@2
	int v6; // ecx@7
	unsigned int v7; // [sp+4h] [bp-18h]@2

	v4 = (signed int)InBuff % -16;
	result = OutBuff;
	if ((signed int)OutBuff % -16 | (signed int)InBuff % -16)
	{
		if (v4 == (signed int)OutBuff % -16)
		{
			v6 = -v4;
			memcpy(OutBuff, InBuff, v6 + 16);
			Decrypt((char *)OutBuff + v6 + 16, (char *)InBuff + v6 + 16, Size - (v6 + 16));
			result = OutBuff;
		}
		else
		{
			memcpy(OutBuff, InBuff, Size);
			result = OutBuff;
		}
	}
	else
	{
		v5 = Size & 0x7F;
		v7 = v5;
		if (Size != v5)
		{
			CreateSBox((int)OutBuff, (int)InBuff, Size - v5);
			result = OutBuff;
			v5 = v7;
		}
		if (v5)
		{
			memcpy(&result + Size - v5, (unsigned int*)InBuff + Size - v5, v7);
			result = OutBuff;
		}
	}
	return result;
}

void *__cdecl OXCManager::DecryptWrap(void *OutBuff, const void *InBuff, unsigned int Size)
{
	void *v3; // edi@1
	const void *v4; // esi@1
	void *result = nullptr; // eax@6
	unsigned int v6; // edx@8
	unsigned int v7; // ecx@8
	int v8; // edi@26
	const void *v9; // esi@26
	unsigned int v10; // edx@27
	unsigned int v11; // ecx@27

	v4 = InBuff;
	v3 = OutBuff;
	if (OutBuff > InBuff && OutBuff < (char *)InBuff + Size)
	{
		v9 = &InBuff + Size - 4;
		v8 = (int)(&OutBuff + Size - 4);
		if (v8 & 3)
		{
			if (Size >= 4)
			{
				switch (Size)
				{
				case 0u:
				LABEL_34 :
					result = OutBuff;
						 break;
				case 1u:
				LABEL_35 :
					*(_BYTE *)(v8 + 3) = *((_BYTE *)v9 + 3);
						 result = OutBuff;
						 break;
				case 2u:
				LABEL_36 :
					*(_BYTE *)(v8 + 3) = *((_BYTE *)v9 + 3);
						 *(_BYTE *)(v8 + 2) = *((_BYTE *)v9 + 2);
						 result = OutBuff;
						 break;
				case 3u:
					goto LABEL_37;
				}
			}
		}
		else
		{
			v11 = Size >> 2;
			v10 = Size & 3;
			if (Size >> 2 < 8)
			{
				switch (v11)
				{
				case 0:
					switch (v10)
					{
					case 0u:
						goto LABEL_34;
					case 1u:
						goto LABEL_35;
					case 2u:
						goto LABEL_36;
					case 3u:
						goto LABEL_37;
					}
				}
			}
			else
			{
				memcpy((void *)v8, v9, 4 * v11);
				v9 = (char *)v9 + 4 * v11;
				v8 += 4 * v11;
				switch (v10)
				{
				case 0u:
					goto LABEL_34;
				case 1u:
					goto LABEL_35;
				case 2u:
					goto LABEL_36;
				case 3u:
					break;
				}
			}
		LABEL_37:
			*(_BYTE *)(v8 + 3) = *((_BYTE *)v9 + 3);
			*(_BYTE *)(v8 + 2) = *((_BYTE *)v9 + 2);
			*(_BYTE *)(v8 + 1) = *((_BYTE *)v9 + 1);
			result = OutBuff;
		}
	}
	else
	{
		if (Size >= 0x100 && OXCManager::UseLowLevelDecryption && (v4 = InBuff, v3 = OutBuff, ((_BYTE)OutBuff & 0xF) == ((_BYTE)InBuff & 0xF)))
		{
			result = OXCManager::Decrypt(OutBuff, InBuff, Size);
		}
		else
		{
			if ((_BYTE)v3 & 3)
			{
				if (Size >= 4)
					goto JUMPOUTLAB;
				else
					goto JUMPOUTLABTWO;
			}
			v7 = Size >> 2;
			v6 = Size & 3;
			if (Size >> 2 < 8)
			{
				JUMPOUTLAB:
				switch (v7)
				{
				case 0u:
					goto LABEL_21;
				case 1u:
					goto LABEL_20;
				case 2u:
					goto LABEL_19;
				case 3u:
					goto LABEL_18;
				case 4u:
					goto LABEL_17;
				case 5u:
					goto LABEL_16;
				case 6u:
					break;
				case 7u:
					*((_DWORD *)v3 + v7 - 7) = *((_DWORD *)v4 + v7 - 7);
					break;
				}
				*((_DWORD *)v3 + v7 - 6) = *((_DWORD *)v4 + v7 - 6);
			LABEL_16:
				*((_DWORD *)v3 + v7 - 5) = *((_DWORD *)v4 + v7 - 5);
			LABEL_17:
				*((_DWORD *)v3 + v7 - 4) = *((_DWORD *)v4 + v7 - 4);
			LABEL_18:
				*((_DWORD *)v3 + v7 - 3) = *((_DWORD *)v4 + v7 - 3);
			LABEL_19:
				*((_DWORD *)v3 + v7 - 2) = *((_DWORD *)v4 + v7 - 2);
			LABEL_20:
				*((_DWORD *)v3 + v7 - 1) = *((_DWORD *)v4 + v7 - 1);
				v4 = (char *)v4 + 4 * v7;
				v3 = (char *)v3 + 4 * v7;
			LABEL_21:
				switch (v6)
				{
				case 0u:
				LABEL_22 :
					JUMPOUTLABTWO:
					result = OutBuff;
						 break;
				case 1u:
				LABEL_23 :
					*(_BYTE *)v3 = *(_BYTE *)v4;
						 result = OutBuff;
						 break;
				case 2u:
				LABEL_24 :
					*(_BYTE *)v3 = *(_BYTE *)v4;
						 *((_BYTE *)v3 + 1) = *((_BYTE *)v4 + 1);
						 result = OutBuff;
						 break;
				case 3u:
					goto LABEL_25;
				}
			}
			else
			{
				memcpy(v3, v4, 4 * v7);
				v4 = (char *)v4 + 4 * v7;
				v3 = (char *)v3 + 4 * v7;
				switch (v6)
				{
				case 0u:
					goto LABEL_22;
				case 1u:
					goto LABEL_23;
				case 2u:
					goto LABEL_24;
				case 3u:
					break;
				}
			LABEL_25:
				*(_BYTE *)v3 = *(_BYTE *)v4;
				*((_BYTE *)v3 + 1) = *((_BYTE *)v4 + 1);
				*((_BYTE *)v3 + 2) = *((_BYTE *)v4 + 2);
				result = OutBuff;
			}
		}
	}
	return result;
}

void OXCManager::SetDecryption(short int Option)
{
	switch (Option)
	{
	case LLD_ENABLE:
		_LLDOPTION = 1;
		break;
	case LLD_DISABLE:
		_LLDOPTION = 0;
		break;

	default:
		_LLDOPTION = 1;
		break;
	}
}