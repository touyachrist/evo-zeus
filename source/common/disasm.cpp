#pragma once
#include <windows.h>


#include "disasm.h"
#include "..\common\str.h"
#include <TlHelp32.h>
#define CWA(dll, api)                 ::api

extern "C" int WINAPI LDE(void *, int);


void Disasm::init(void)
{

}

void Disasm::uninit(void)
{

}

DWORD Disasm::_getOpcodeLength(void *pAddress)
{
#if defined _WIN64  
  return (DWORD)LDE(pAddress, 64);
#else
  return (DWORD)LDE(pAddress, 0);
#endif
}

#define _SALC	0xD6
#define _AAM	0xD4
#define NRM_TAB_LEN	53

#define DB __asm _emit



DWORD Disasm::GetModuleSize(HMODULE moduleHandle)
{
	MODULEINFO a;
	GetModuleInformation(GetCurrentProcess(),moduleHandle,&a,sizeof(a));
	return a.SizeOfImage;

}

 bool Disasm::DataCompare( const BYTE* pData, const BYTE* bMask, const char* szMask )
{
	for( ; *szMask; ++szMask, ++pData, ++bMask )
	{
		if( *szMask == 'x' && *pData != *bMask )
			return false;
	}

	return ( *szMask ) == NULL;
}

// Finds a pattern at the specified address
 DWORD Disasm::FindPattern ( DWORD dwAddress, DWORD dwSize, BYTE* pbMask, char* szMask )
{
	for( DWORD i = NULL; i < dwSize; i++ )
	{
		if( DataCompare( (BYTE*) ( dwAddress + i ), pbMask, szMask ) )
			return (DWORD)( dwAddress + i );
	}
	return 0;
}

 DWORD Splice::GetBuffer()
	{
		return(DWORD)buffer;
	}
