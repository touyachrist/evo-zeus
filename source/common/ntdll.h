#pragma once
/*
  Набор определений для ntdll.dll
*/

#if !defined NTDLL_IMPORT
  #error NTDLL_IMPORT not defined!
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// NT статусы.
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef LONG NTSTATUS;

// Generic test for success on any status value (non-negative numbers indicate success).
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

// Generic test for information on any status value.
#define NT_INFORMATION(Status) ((((ULONG)(Status)) >> 30) == 1)

// Generic test for warning on any status value.
#define NT_WARNING(Status) ((((ULONG)(Status)) >> 30) == 2)

// Generic test for error on any status value.
#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)

////////////////////////////////////////////////////////////////////////////////////////////////////

//The success status codes.
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#define STATUS_ACCESS_DENIED			 ((NTSTATUS)0xC0000022L)


//The file %hs does not exist.
#define STATUS_NO_SUCH_FILE              ((NTSTATUS)0xC000000FL)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Разные определния.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef LONG KPRIORITY;

//Флаги для NtCreateUserProcess.CreateProcessFlags
#define CREATE_PROCESS_INHERIT_HANDLES 0x0004
#define CREATE_PROCESS_PROTECTED       0x0040

//Флаги для NtCreateUserProcess.CreateThreadFlags
#define CREATE_THREAD_SUSPENDED 0x0001

//Режимы создания файла.
#define FILE_SUPERSEDE           0x00000000
#define FILE_OPEN                0x00000001
#define FILE_CREATE              0x00000002
#define FILE_OPEN_IF             0x00000003
#define FILE_OVERWRITE           0x00000004
#define FILE_OVERWRITE_IF        0x00000005
#define FILE_MAXIMUM_DISPOSITION 0x00000005

//Опции создания файла.
#define FILE_DIRECTORY_FILE            0x00000001
#define FILE_WRITE_THROUGH             0x00000002
#define FILE_SEQUENTIAL_ONLY           0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING 0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT      0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT   0x00000020
#define FILE_NON_DIRECTORY_FILE        0x00000040
#define FILE_CREATE_TREE_CONNECTION    0x00000080

#define FILE_COMPLETE_IF_OPLOCKED      0x00000100
#define FILE_NO_EA_KNOWLEDGE           0x00000200
#define FILE_OPEN_REMOTE_INSTANCE      0x00000400
#define FILE_RANDOM_ACCESS             0x00000800

#define FILE_DELETE_ON_CLOSE           0x00001000
#define FILE_OPEN_BY_FILE_ID           0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT    0x00004000
#define FILE_NO_COMPRESSION            0x00008000

#define FILE_RESERVE_OPFILTER          0x00100000
#define FILE_OPEN_REPARSE_POINT        0x00200000
#define FILE_OPEN_NO_RECALL            0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY 0x00800000

typedef struct
{
  void *functionForHook;
  void *hookerFunction;
  void *originalFunction;
  BYTE originalFunctionSize;
}HOOKWINAPI;

typedef enum
{
  ProcessBasicInformation,
  ProcessQuotaLimits,
  ProcessIoCounters,
  ProcessVmCounters,
  ProcessTimes,
  ProcessBasePriority,
  ProcessRaisePriority,
  ProcessDebugPort,
  ProcessExceptionPort,
  ProcessAccessToken,
  ProcessLdtInformation,
  ProcessLdtSize,
  ProcessDefaultHardErrorMode,
  ProcessIoPortHandlers,          // Note: this is kernel mode only
  ProcessPooledUsageAndLimits,
  ProcessWorkingSetWatch,
  ProcessUserModeIOPL,
  ProcessEnableAlignmentFaultFixup,
  ProcessPriorityClass,
  ProcessWx86Information,
  ProcessHandleCount,
  ProcessAffinityMask,
  ProcessPriorityBoost,
  ProcessDeviceMap,
  ProcessSessionInformation,
  ProcessForegroundInformation,
  ProcessWow64Information,
  ProcessImageFileName,
  ProcessLUIDDeviceMapsEnabled,
  ProcessBreakOnTermination,
  ProcessDebugObjectHandle,
  ProcessDebugFlags,
  ProcessHandleTracing,
  ProcessIoPriority,
  ProcessExecuteFlags,
  ProcessTlsInformation,
  ProcessCookie,
  ProcessImageInformation,
  ProcessCycleTime,
  ProcessPagePriority,
  ProcessInstrumentationCallback,
  ProcessThreadStackAllocation,
  ProcessWorkingSetWatchEx,
  ProcessImageFileNameWin32,
  ProcessImageFileMapping,
  MaxProcessInfoClass             // MaxProcessInfoClass should always be the last enum
}PROCESSINFOCLASS;

typedef enum
{
  FileDirectoryInformation         = 1,
  FileFullDirectoryInformation,   // 2
  FileBothDirectoryInformation,   // 3
  FileBasicInformation,           // 4
  FileStandardInformation,        // 5
  FileInternalInformation,        // 6
  FileEaInformation,              // 7
  FileAccessInformation,          // 8
  FileNameInformation,            // 9
  FileRenameInformation,          // 10
  FileLinkInformation,            // 11
  FileNamesInformation,           // 12
  FileDispositionInformation,     // 13
  FilePositionInformation,        // 14
  FileFullEaInformation,          // 15
  FileModeInformation,            // 16
  FileAlignmentInformation,       // 17
  FileAllInformation,             // 18
  FileAllocationInformation,      // 19
  FileEndOfFileInformation,       // 20
  FileAlternateNameInformation,   // 21
  FileStreamInformation,          // 22
  FilePipeInformation,            // 23
  FilePipeLocalInformation,       // 24
  FilePipeRemoteInformation,      // 25
  FileMailslotQueryInformation,   // 26
  FileMailslotSetInformation,     // 27
  FileCompressionInformation,     // 28
  FileObjectIdInformation,        // 29
  FileCompletionInformation,      // 30
  FileMoveClusterInformation,     // 31
  FileQuotaInformation,           // 32
  FileReparsePointInformation,    // 33
  FileNetworkOpenInformation,     // 34
  FileAttributeTagInformation,    // 35
  FileTrackingInformation,        // 36
  FileIdBothDirectoryInformation, // 37
  FileIdFullDirectoryInformation, // 38
  FileValidDataLengthInformation, // 39
  FileShortNameInformation,       // 40
  FileIoCompletionNotificationInformation, // 41
  FileIoStatusBlockRangeInformation,       // 42
  FileIoPriorityHintInformation,           // 43
  FileSfioReserveInformation,              // 44
  FileSfioVolumeInformation,               // 45
  FileHardLinkInformation,                 // 46
  FileProcessIdsUsingFileInformation,      // 47
  FileNormalizedNameInformation,           // 48
  FileNetworkPhysicalNameInformation,      // 49
  FileMaximumInformation
}FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef enum
{
  SystemBasicInformation,
  SystemProcessorInformation,
  SystemPerformanceInformation,
  SystemTimeOfDayInformation,
  SystemPathInformation,
  SystemProcessInformation,
  SystemCallCountInformation,
  SystemDeviceInformation,
  SystemProcessorPerformanceInformation,
  SystemFlagsInformation,
  SystemCallTimeInformation,
  SystemModuleInformation, 
  SystemLocksInformation,
  SystemStackTraceInformation,
  SystemPagedPoolInformation,
  SystemNonPagedPoolInformation,
  SystemHandleInformation,
  SystemObjectInformation,
  SystemPageFileInformation,
  SystemVdmInstemulInformation,
  SystemVdmBopInformation,
  SystemFileCacheInformation,
  SystemPoolTagInformation,
  SystemInterruptInformation,
  SystemDpcBehaviorInformation,
  SystemFullMemoryInformation,
  SystemLoadGdiDriverInformation,
  SystemUnloadGdiDriverInformation,
  SystemTimeAdjustmentInformation,
  SystemSummaryMemoryInformation,
  SystemNextEventIdInformation,
  SystemEventIdsInformation,
  SystemCrashDumpInformation,
  SystemExceptionInformation,
  SystemCrashDumpStateInformation,
  SystemKernelDebuggerInformation,
  SystemContextSwitchInformation,
  SystemRegistryQuotaInformation,
  SystemExtendServiceTableInformation,
  SystemPrioritySeperation,
  SystemPlugPlayBusInformation,
  SystemDockInformation,
  SystemPowerInformation1, //Конфиликт с winnt.h
  SystemProcessorSpeedInformation,
  SystemCurrentTimeZoneInformation,
  SystemLookasideInformation
}SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

typedef enum
{
    MemoryBasicInformation,
    MemoryWorkingSetList,
    MemorySectionName,
    MemoryBasicVlmInformation
} MEMORY_INFORMATION_CLASS, *PMEMORY_INFORMATION_CLASS;

typedef enum
{
  ObjectBasicInformation,
  ObjectNameInformation,
  ObjectTypeInformation,
  ObjectAllInformation,
  ObjectDataInformation
}OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Структуры.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
  USHORT Length;
  USHORT MaximumLength;
  LPWSTR Buffer;
}UNICODE_STRING, *PUNICODE_STRING;

typedef struct
{
  USHORT Length;
  USHORT MaximumLength;
  PCHAR  Buffer;
}ANSI_STRING, *PANSI_STRING;

typedef struct
{
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
  PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
}OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct
{
  HANDLE UniqueProcess;
  HANDLE UniqueThread;
}CLIENT_ID, *PCLIENT_ID;

typedef struct
{
  PVOID StackBase;
  PVOID StackLimit;
  PVOID StackCommit;
  PVOID StackCommitMax;
  PVOID StackReserved;
}INITIAL_TEB, *PINITIAL_TEB;

typedef struct
{
  union
  {
    NTSTATUS Status;
    PVOID Pointer;
  };
  ULONG_PTR Information;
}IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct
{
  UNICODE_STRING Name;
}OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef struct
{
  NTSTATUS ExitStatus;
  void *PebBaseAddress;
  ULONG_PTR AffinityMask;
  KPRIORITY BasePriority;
  ULONG_PTR UniqueProcessId;
  ULONG_PTR InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

typedef struct
{
  ULONG NextEntryOffset;
  ULONG FileIndex;
  ULONG FileNameLength;
  WCHAR FileName[1];
}FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

typedef struct
{
  ULONG NextEntryOffset;
  ULONG FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG FileAttributes;
  ULONG FileNameLength;
  WCHAR FileName[1];
}FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _PEB_FREE_BLOCK
{
   struct _PEB_FREE_BLOCK	*Next;
   ULONG					Size;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

typedef VOID NTSYSAPI (*PPEBLOCKROUTINE)(PVOID);

typedef ULONG_PTR KSPIN_LOCK;
typedef KSPIN_LOCK *PKSPIN_LOCK;

typedef struct _SYSTEM_STRINGS
{
	UNICODE_STRING	SystemRoot;       // C:\WINNT
	UNICODE_STRING	System32Root;     // C:\WINNT\System32
	UNICODE_STRING	BaseNamedObjects; // \BaseNamedObjects
}SYSTEM_STRINGS,*PSYSTEM_STRINGS;

typedef struct _TEXT_INFO
{
	PVOID			Reserved;
	PSYSTEM_STRINGS	SystemStrings;
}TEXT_INFO, *PTEXT_INFO;

typedef struct _RTL_BITMAP
{
	DWORD	SizeOfBitMap;
	PDWORD	Buffer;
} RTL_BITMAP, *PRTL_BITMAP, **PPRTL_BITMAP;

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength), length_is(Length) ]
#endif // MIDL_PASS
    PCHAR Buffer;
} STRING, *PSTRING;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	WORD	Flags;
	WORD	Length;
	DWORD	TimeStamp;
	STRING	DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct _CURDIR
{
   UNICODE_STRING	DosPath;
   HANDLE			Handle;
} CURDIR, *PCURDIR;

typedef struct _PROCESS_PARAMETERS
{
    ULONG					MaximumLength;
    ULONG					Length;
    ULONG					Flags;				// PROCESS_PARAMETERS_NORMALIZED
    ULONG					DebugFlags;
    HANDLE					ConsoleHandle;
    ULONG					ConsoleFlags;
    HANDLE					StandardInput;
    HANDLE					StandardOutput;
    HANDLE					StandardError;
    CURDIR					CurrentDirectory;
    UNICODE_STRING			DllPath;
    UNICODE_STRING			ImagePathName;
    UNICODE_STRING			CommandLine;
    PWSTR					Environment;
    ULONG					StartingX;
    ULONG					StartingY;
    ULONG					CountX;
    ULONG					CountY;
    ULONG					CountCharsX;
    ULONG					CountCharsY;
    ULONG					FillAttribute;
    ULONG					WindowFlags;
    ULONG					ShowWindowFlags;
    UNICODE_STRING			WindowTitle;
    UNICODE_STRING			Desktop;
    UNICODE_STRING			ShellInfo;
    UNICODE_STRING			RuntimeInfo;
	RTL_DRIVE_LETTER_CURDIR	CurrentDirectores[32];
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;

typedef struct _PEB_LDR_DATA
{
	ULONG		Length;
	BOOLEAN		Initialized;
	PVOID		SsHandle;
	LIST_ENTRY	InLoadOrderModuleList; // ref. to PLDR_DATA_TABLE_ENTRY->InLoadOrderModuleList
	LIST_ENTRY	InMemoryOrderModuleList; // ref. to PLDR_DATA_TABLE_ENTRY->InMemoryOrderModuleList
	LIST_ENTRY	InInitializationOrderModuleList; // ref. to PLDR_DATA_TABLE_ENTRY->InInitializationOrderModuleList
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB
{
	UCHAR				InheritedAddressSpace;				// 0
	UCHAR				ReadImageFileExecOptions;			// 1
	UCHAR				BeingDebugged;						// 2
	BYTE				b003;								// 3
	PVOID				Mutant;								// 4
	PVOID				ImageBaseAddress;					// 8
	PPEB_LDR_DATA		Ldr;								// C
	PPROCESS_PARAMETERS	ProcessParameters;					// 10
	PVOID				SubSystemData;						// 14  
	PVOID				ProcessHeap;						// 18
	KSPIN_LOCK			FastPebLock;						// 1C
	PPEBLOCKROUTINE		FastPebLockRoutine;					// 20
	PPEBLOCKROUTINE		FastPebUnlockRoutine;				// 24
	ULONG				EnvironmentUpdateCount;				// 28
	PVOID				*KernelCallbackTable;				// 2C
	PVOID				EventLogSection;					// 30
	PVOID				EventLog;							// 34
	PPEB_FREE_BLOCK		FreeList;							// 38
	ULONG				TlsExpansionCounter;				// 3C
	PRTL_BITMAP			TlsBitmap;							// 40
	ULONG				TlsBitmapData[0x2];					// 44
	PVOID				ReadOnlySharedMemoryBase;			// 4C
	PVOID				ReadOnlySharedMemoryHeap;			// 50
	PTEXT_INFO			ReadOnlyStaticServerData;			// 54
	PVOID				InitAnsiCodePageData;				// 58
	PVOID				InitOemCodePageData;				// 5C
	PVOID				InitUnicodeCaseTableData;			// 60
	ULONG				KeNumberProcessors;					// 64
	ULONG				NtGlobalFlag;						// 68
	DWORD				d6C;								// 6C
	LARGE_INTEGER		MmCriticalSectionTimeout;			// 70
	ULONG				MmHeapSegmentReserve;				// 78
	ULONG				MmHeapSegmentCommit;				// 7C
	ULONG				MmHeapDeCommitTotalFreeThreshold;	// 80
	ULONG				MmHeapDeCommitFreeBlockThreshold;	// 84
	ULONG				NumberOfHeaps;						// 88
	ULONG				AvailableHeaps;						// 8C
	PHANDLE				ProcessHeapsListBuffer;				// 90
	PVOID				GdiSharedHandleTable;				// 94
	PVOID				ProcessStarterHelper;				// 98
	PVOID				GdiDCAttributeList;					// 9C
	KSPIN_LOCK			LoaderLock;							// A0
	ULONG				NtMajorVersion;						// A4
	ULONG				NtMinorVersion;						// A8
	USHORT				NtBuildNumber;						// AC
	USHORT				NtCSDVersion;						// AE
	ULONG				PlatformId;							// B0
	ULONG				Subsystem;							// B4
	ULONG				MajorSubsystemVersion;				// B8
	ULONG				MinorSubsystemVersion;				// BC
	KAFFINITY			AffinityMask;						// C0
	ULONG				GdiHandleBuffer[0x22];				// C4
	ULONG				PostProcessInitRoutine;				// 14C
	ULONG				TlsExpansionBitmap;					// 150
	UCHAR				TlsExpansionBitmapBits[0x80];		// 154
	ULONG				SessionId;							// 1D4
	ULARGE_INTEGER		AppCompatFlags;						// 1D8
	PWORD				CSDVersion;							// 1E0
/*	PVOID				AppCompatInfo;						// 1E4
	UNICODE_STRING		usCSDVersion;
	PVOID				ActivationContextData;
    PVOID				ProcessAssemblyStorageMap;
    PVOID				SystemDefaultActivationContextData;
    PVOID				SystemAssemblyStorageMap;
    ULONG				MinimumStackCommit; */
} PEB, *PPEB;

typedef struct _TEB
{
	NT_TIB			Tib;                         
	PVOID			EnvironmentPointer;           
	CLIENT_ID		Cid;                      
	PVOID			ActiveRpcInfo;                
	PVOID			ThreadLocalStoragePointer;    
	PPEB			Peb;                           
	ULONG			LastErrorValue;               
	ULONG			CountOfOwnedCriticalSections; 
	PVOID			CsrClientThread;              
	PVOID			Win32ThreadInfo;              
	ULONG			Win32ClientInfo[0x1F];        
	PVOID			WOW32Reserved;                
	ULONG			CurrentLocale;                
	ULONG			FpSoftwareStatusRegister;     
	PVOID			SystemReserved1[0x36];        
	PVOID			Spare1;                       
	LONG			ExceptionCode;                 
	ULONG			SpareBytes1[0x28];            
	PVOID			SystemReserved2[0xA];         
	ULONG			gdiRgn;                       
	ULONG			gdiPen;                       
	ULONG			gdiBrush;                     
	CLIENT_ID		RealClientId;             
	PVOID			GdiCachedProcessHandle;       
	ULONG			GdiClientPID;                 
	ULONG			GdiClientTID;                 
	PVOID			GdiThreadLocaleInfo;          
	PVOID			UserReserved[5];              
	PVOID			glDispatchTable[0x118];       
	ULONG			glReserved1[0x1A];            
	PVOID			glReserved2;                  
	PVOID			glSectionInfo;                
	PVOID			glSection;                    
	PVOID			glTable;                      
	PVOID			glCurrentRC;                  
	PVOID			glContext;                    
	NTSTATUS		LastStatusValue;           
	UNICODE_STRING	StaticUnicodeString; 
	WCHAR			StaticUnicodeBuffer[0x105];   
	PVOID			DeallocationStack;            
	PVOID			TlsSlots[0x40];               
	LIST_ENTRY		TlsLinks;                
	PVOID			Vdm;                          
	PVOID			ReservedForNtRpc;             
	PVOID			DbgSsReserved[0x2];           
	ULONG			HardErrorDisabled;            
	PVOID			Instrumentation[0x10];        
	PVOID			WinSockData;                  
	ULONG			GdiBatchCount;                
	ULONG			Spare2;                       
	ULONG			Spare3;                       
	ULONG			Spare4;                       
	PVOID			ReservedForOle;               
	ULONG			WaitingOnLoaderLock;          
	PVOID			StackCommit;                  
	PVOID			StackCommitMax;               
	PVOID			StackReserve;                 
} TEB, *PTEB;

typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS	ExitStatus;
    PTEB		TebBaseAddress;
    CLIENT_ID	ClientId;
    KAFFINITY	AffinityMask;
    KPRIORITY	Priority;
    KPRIORITY	BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef enum _THREADINFOCLASS
{
	ThreadBasicInformation,
	ThreadTimes,				// KERNEL_USER_TIMES
	ThreadPriority,
	ThreadBasePriority,			// BASE_PRIORITY_INFORMATION
	ThreadAffinityMask,			// AFFINITY_MASK
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,			// W2K
	ThreadHideFromDebugger,		// W2K
	MaxThreadInfoClass
} THREADINFOCLASS;

typedef struct
{
  ULONG NextEntryOffset;
  ULONG FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG FileAttributes;
  ULONG FileNameLength;
  ULONG EaSize;
  WCHAR FileName[1];
}FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

typedef struct
{
  ULONG NextEntryOffset;
  ULONG FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG FileAttributes;
  ULONG FileNameLength;
  ULONG EaSize;
  LARGE_INTEGER FileId;
  WCHAR FileName[1];
}FILE_ID_FULL_DIR_INFORMATION, *PFILE_ID_FULL_DIR_INFORMATION;

typedef struct
{
  ULONG NextEntryOffset;
  ULONG FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG FileAttributes;
  ULONG FileNameLength;
  ULONG EaSize;
  CCHAR ShortNameLength;
  WCHAR ShortName[12];
  WCHAR FileName[1];
}FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct
{
  ULONG NextEntryOffset;
  ULONG FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG FileAttributes;
  ULONG FileNameLength;
  ULONG EaSize;
  CCHAR ShortNameLength;
  WCHAR ShortName[12];
  LARGE_INTEGER FileId;
  WCHAR FileName[1];
}FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;

typedef struct _MEMORY_WORKING_SET_LIST
{
	ULONG	NumberOfPages;
	ULONG	WorkingSetList[1];
} MEMORY_WORKING_SET_LIST, *PMEMORY_WORKING_SET_LIST;


////////////////////////////////////////////////////////////////////////////////////////////////////
// Функции.
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (NTAPI *PIO_APC_ROUTINE)
(
 PVOID ApcContext,
 PIO_STATUS_BLOCK IoStatusBlock,
 ULONG Reserved
 );

#if(NTDLL_IMPORT == 1)
  #define DECLARE_NTAPI(ret, name) __declspec(dllimport) ret NTAPI name
#else
  #define DECLARE_NTAPI(ret, name) typedef ret (NTAPI *ntdll##name)
#endif

#if(NTDLL_IMPORT == 1)
extern "C"
{
#endif


  DECLARE_NTAPI(NTSTATUS, CsrClientCallServer)(
	PVOID Arg1,
	PVOID Arg2, 
	ULONG Arg3, 
	ULONG Arg4
  );

  DECLARE_NTAPI(NTSTATUS, NtSetInformationThread)(
    __in HANDLE ThreadHandle, 
    __in THREADINFOCLASS ThreadInformationClass,
    __in PVOID ThreadInformation,
    __in ULONG ThreadInformationLength
  );

  DECLARE_NTAPI(NTSTATUS, NtCreateThread)
  (
    PHANDLE ThreadHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    HANDLE ProcessHandle,
    PCLIENT_ID ClientId,
    PCONTEXT ThreadContext,
    PINITIAL_TEB InitialTeb,
    BOOLEAN CreateSuspended
  );

  DECLARE_NTAPI(NTSTATUS, NtResumeThread)
  (
	HANDLE               ThreadHandle,
	PULONG               SuspendCount
  );

  DECLARE_NTAPI(NTSTATUS, NtOpenProcess)
  (
	OUT PHANDLE				phProcess,
	IN ACCESS_MASK			AccessMask,
	IN POBJECT_ATTRIBUTES	pObjectAttributes,
	IN PCLIENT_ID			pClientId
  );

  typedef NTSTATUS (WINAPI *NtQueueApcThread)(HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,ULONG);

  DECLARE_NTAPI(NTSTATUS, NtQueryInformationProcess)
  (
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass, 
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
  );

  DECLARE_NTAPI(NTSTATUS, LdrLoadDll)
  (
    PWCHAR PathToFile,
    ULONG Flags,
    PUNICODE_STRING ModuleFileName,
    PHANDLE ModuleHandle
  );

  DECLARE_NTAPI(NTSTATUS, LdrGetDllHandle)
  (
    PWCHAR PathToFile,
    PVOID Unused,
    PUNICODE_STRING ModuleFileName,
    PHANDLE ModuleHandle
  );

  DECLARE_NTAPI(NTSTATUS, LdrGetProcedureAddress)
  (
    HMODULE ModuleHandle,
    PANSI_STRING FunctionName,
    WORD Oridinal,
    PVOID *FunctionAddress
  );

  DECLARE_NTAPI(NTSTATUS, NtQueryDirectoryFile)
  (
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    BOOLEAN ReturnSingleEntry,
    PUNICODE_STRING FileName,
    BOOLEAN RestartScan
  );

	DECLARE_NTAPI(NTSTATUS, NtQueryInformationThread)
	(
		IN HANDLE			hThread,
		IN THREADINFOCLASS	ThreadInformationClass,
		OUT PVOID			pThreadInformation,
		IN ULONG			uThreadInformationLength,
		OUT PULONG			puReturnLength OPTIONAL
	);
	  
  DECLARE_NTAPI(NTSTATUS, NtReadVirtualMemory)
  (
	HANDLE              ProcessHandle,
	PVOID               BaseAddress,
	PVOID               Buffer,
	ULONG               NumberOfBytesToRead,
	PULONG              NumberOfBytesReaded
  );

  DECLARE_NTAPI(NTSTATUS, NtQueryVirtualMemory)
  (
	HANDLE				ProcessHandle,
    PVOID				BaseAddress,
    MEMORY_INFORMATION_CLASS				MemoryInformationClass,
    PVOID				MemoryInformation,
    ULONG				MemoryInformationLength,
    PULONG				ReturnLength
  );

  DECLARE_NTAPI(NTSTATUS, NtMapViewOfSection)
  (
	HANDLE,HANDLE,LPVOID,ULONG,SIZE_T,LARGE_INTEGER*,SIZE_T*,DWORD,ULONG,ULONG
  );

  DECLARE_NTAPI(NTSTATUS, NtQuerySystemInformation)
  (
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
  );

  DECLARE_NTAPI(NTSTATUS, NtQueryObject)
  (
    HANDLE Handle,
    OBJECT_INFORMATION_CLASS ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
  );

  DECLARE_NTAPI(NTSTATUS, NtCreateFile)
  (
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer,
    ULONG EaLength
  );

  DECLARE_NTAPI(NTSTATUS, RtlCreateUserThread)
  (
    HANDLE ProcessHandle,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN CreateSuspended,
    ULONG StackZeroBits,
    PULONG StackReserved,
    PULONG StackCommit,
    PVOID StartAddress,
    PVOID StartParameter,
    PHANDLE ThreadHandle,
    PVOID ClientID
  );

  DECLARE_NTAPI(NTSTATUS, NtCreateUserProcess)
  (
    PHANDLE ProcessHandle,
    PHANDLE ThreadHandle,
    ACCESS_MASK ProcessDesiredAccess,
    ACCESS_MASK ThreadDesiredAccess,
    POBJECT_ATTRIBUTES ProcessObjectAttributes,
    POBJECT_ATTRIBUTES ThreadObjectAttributes,
    ULONG CreateProcessFlags,
    ULONG CreateThreadFlags,
    PVOID /*PRTL_USER_PROCESS_PARAMETERS*/ ProcessParameters,
    PVOID Parameter9,
    PVOID /*PNT_PROC_THREAD_ATTRIBUTE_LIST*/ AttributeList
  );

  DECLARE_NTAPI(NTSTATUS, NtResumeThread)
  (
    HANDLE ThreadHandle,
    PULONG SuspendCount
  );

  DECLARE_NTAPI(NTSTATUS, NtCreateFile)
  (
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer,
    ULONG EaLength
  );

#if(NTDLL_IMPORT == 1)
};
#endif

#undef DECLARE_NTAPI

