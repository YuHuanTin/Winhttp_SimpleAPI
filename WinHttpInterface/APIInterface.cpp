#include "APIInterface.h"

using std::string;
using std::wstring;

DWORD WinHttpInterface::Open(const wstring &UA, const wstring &Proxy, const wstring &ProxyBypass, DWORD Flags) {
    hSession = WinHttpOpen(UA.c_str(),
                           Proxy.empty() ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY : WINHTTP_ACCESS_TYPE_NAMED_PROXY,
                           Proxy.empty() ? WINHTTP_NO_PROXY_NAME : Proxy.c_str(),
                           ProxyBypass.empty() ? WINHTTP_NO_PROXY_BYPASS : ProxyBypass.c_str(),
                           Flags
    );
    if (hSession == nullptr)
        return GetLastError();
    return 0;
}
DWORD WinHttpInterface::CrackUrl(const wstring &Url, DWORD UrlLen, DWORD Flags, URL_COMPONENTS &UrlComponents){
    if (!WinHttpCrackUrl(Url.c_str(),
                         UrlLen,
                         Flags,
                         &UrlComponents
    )){
        return GetLastError();
    }
    return 0;
}
DWORD WinHttpInterface::Connect(const wstring &HostName, INTERNET_PORT Port) {
    hConnect = WinHttpConnect(hSession,
                              HostName.c_str(),
                              Port,
                              0
    );
    if (hConnect == nullptr)
        return GetLastError();
    return 0;
}
DWORD WinHttpInterface::OpenRequest(const wstring &Verb, const wstring &UrlPath, const wstring &Version, DWORD Flags) {
    hRequest = WinHttpOpenRequest(hConnect,
                                  Verb.c_str(),
                                  UrlPath.c_str(),
                                  Version.c_str(),
                                  nullptr,
                                  nullptr,
                                  Flags
    );
    if (hRequest == nullptr)
        return GetLastError();
    return 0;
}
DWORD WinHttpInterface::SetOption(DWORD Option, DWORD Buffer) {
    if (!WinHttpSetOption(hRequest,
                          Option,
                          &Buffer,
                          sizeof(Buffer)
    )){
        return GetLastError();
    }
    return 0;
}
DWORD WinHttpInterface::SetTimeOut(int ResolveTimeout, int ConnectTimeout, int SendTimeout, int ReceiveTimeout) {
    if (!WinHttpSetTimeouts(hRequest,
                            ResolveTimeout,
                            ConnectTimeout,
                            SendTimeout,
                            ReceiveTimeout
    )){
        return GetLastError();
    }
    return 0;
}
DWORD WinHttpInterface::AddRequestHeaders(const wstring &Headers, DWORD Modifiers) {
    if (!WinHttpAddRequestHeaders(hRequest,
                                  Headers.c_str(),//split by '\r\n'
                                  Headers.length(),
                                  Modifiers == 0 ? WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE : Modifiers
    )){
        return GetLastError();
    }
    return 0;
}
DWORD WinHttpInterface::SendRequest(const wstring &Headers, const string &Optional, DWORD OptionalLength, DWORD TotalLength) {
    if (!WinHttpSendRequest(hRequest,
                            Headers.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : Headers.c_str(),
                            Headers.length(),
                            Optional.empty() ? WINHTTP_NO_REQUEST_DATA : (LPVOID)Optional.c_str(),
                            OptionalLength,
                            TotalLength,
                            0
    )){
        return GetLastError();
    }
    return 0;
}
DWORD WinHttpInterface::ReceiveResponse() {
    if (!WinHttpReceiveResponse(hRequest, nullptr))
        return GetLastError();
    return 0;
}
DWORD WinHttpInterface::QueryHeaders(wchar_t *Buffer, DWORD BufferLen = 0) {
    WinHttpQueryHeaders(hRequest,
                        WINHTTP_QUERY_RAW_HEADERS_CRLF,
                        nullptr,
                        Buffer,
                        &BufferLen,
                        nullptr
    );
    return BufferLen;
}
DWORD WinHttpInterface::QueryDataAvailable(DWORD &BufferLen) {
    if (!WinHttpQueryDataAvailable(hRequest,&BufferLen))
        return GetLastError();
    return 0;
}
DWORD WinHttpInterface::ReadData(char *Buffer, DWORD BufferLen = 0) {
    DWORD mNumberOfBytesToRead = 0;
    if (!WinHttpReadData(hRequest,
                         Buffer,
                         BufferLen,
                         &mNumberOfBytesToRead
    )){
        return GetLastError();
    }
    return mNumberOfBytesToRead;
}
void WinHttpInterface::CloseAllHandles() {
    if (hRequest)
        WinHttpCloseHandle(hRequest);
    if (hConnect)
        WinHttpCloseHandle(hConnect);
    if (hSession)
        WinHttpCloseHandle(hSession);
}