/*
  Дизассамблер на движке BeaEngine.
  http://beatrix2004.free.fr/BeaEngine/

  ПРИМИЧАНИЕ: Движок доволно грамозткий, однако это компенсируется его унивирсальностю под x32,
              x64.
*/
#pragma once
//#include <TLHELP32.H>
//
#include "..\common\mem.h"
//
//

#undef RtlMoveMemory
namespace Disasm
{
  /*
    Инициализация.
  */
	void init(void);
	bool IsInProcess(DWORD dwProcessId, LPCSTR ProcessName);
	DWORD _cdecl MDAL_GetOpcodeLen(BYTE* opcode);
	DWORD MDAL_GetOpcodesLenByNeedLen(BYTE* opcode, DWORD NeedLen);
	DWORD GetModuleSize(HMODULE moduleHandle);
	bool DataCompare( const BYTE* pData, const BYTE* bMask, const char* szMask );
	// Finds a pattern at the specified address
	DWORD FindPattern ( DWORD dwAddress, DWORD dwSize, BYTE* pbMask, char* szMask );
  /*
    Деинициализация.
  */
  void uninit(void);

  /*
    Получение длины опкода.

    IN pAddress - адрес опкода.

    Return - длина опкода, или ((DWORD)-1) в случаи ошибки.
  */
  DWORD _getOpcodeLength(void *pAddress);
};
//
//
//bool IsInProcess(DWORD dwProcessId, LPCSTR ProcessName);
//

class Splice
{
public:
	static const int len_jmp=5;
	void* buffer;
	int len_buffer;
	DWORD DestFunc;
	DWORD NewFunc;


	DWORD GetBuffer();

	
};

