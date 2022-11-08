#ifndef WINHTTP_SIMPLEAPI_APIINTERFACE_H
#define WINHTTP_SIMPLEAPI_APIINTERFACE_H

#include <string>
#include "windows.h"
#include "winhttp.h"
#pragma comment(lib,"winhttp.lib")

class WinHttpInterface{
private:
    HINTERNET   hSession = nullptr,
                hConnect = nullptr,
                hRequest = nullptr;
public:
    DWORD Open(const std::wstring &UA, const std::wstring &Proxy, const std::wstring &ProxyBypass, DWORD Flags);
    DWORD CrackUrl(const std::wstring &Url, DWORD UrlLen, DWORD Flags, URL_COMPONENTS &UrlComponents);
    DWORD Connect(const std::wstring &HostName, INTERNET_PORT Port);
    DWORD OpenRequest(const std::wstring &Verb, const std::wstring &UrlPath, const std::wstring &Version, DWORD Flags);
    DWORD SetOption(DWORD Option, DWORD Buffer);
    DWORD SetTimeOut(int ResolveTimeout, int ConnectTimeout, int SendTimeout, int ReceiveTimeout);
    DWORD AddRequestHeaders(const std::wstring &Headers, DWORD Modifiers);
    DWORD SendRequest(const std::wstring &Headers, const std::string &Optional, DWORD OptionalLength, DWORD TotalLength);
    DWORD ReceiveResponse();
    DWORD QueryHeaders(wchar_t *Buffer, DWORD BufferLen);
    DWORD QueryDataAvailable(DWORD &BufferLen);
    DWORD ReadData(char *Buffer, DWORD BufferLen);
    void CloseAllHandles();
};

#endif //WINHTTP_SIMPLEAPI_APIINTERFACE_H
