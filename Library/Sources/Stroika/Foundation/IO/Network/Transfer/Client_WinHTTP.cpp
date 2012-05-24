/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#if     qHasFeature_WinHTTP
#include    <windows.h>
#include    <Winhttp.h>
#endif

#include    "../../../Characters/Format.h"
#include    "../../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../../Execution/Platform/Windows/Exception.h"
#endif
#include    "../HTTP/Headers.h"

#include    "Client_WinHTTP.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::HTTP;
using   namespace   Stroika::Foundation::IO::Network::Transfer;


#if     qPlatform_Windows
using   Stroika::Foundation::Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif
using   Stroika::Foundation::Memory::SmallStackBuffer;






#if     qHasFeature_WinHTTP
class   Connection_WinHTTP::Rep_ : public _IRep {
private:
    NO_COPY_CONSTRUCTOR (Rep_);
    NO_ASSIGNMENT_OPERATOR (Rep_);
public:
    Rep_ ();
    virtual ~Rep_ ();

public:
    virtual DurationSecondsType GetTimeout () const override;
    virtual void                SetTimeout (DurationSecondsType timeout) override;
    virtual URL                 GetURL () const override;
    virtual void                SetURL (const URL& url) override;
    virtual void                Close ()    override;
    virtual Response            SendAndRequest (const Request& request) override;

private:
    nonvirtual  void    AssureHasHandle_ ();

private:
    void*   fSessionHandle_;
    void*   fConnectionHandle_;
};
#endif







#if     qHasFeature_WinHTTP

// otherwise modules linking with this code will tend to get link errors without explicitly linking
// to this module...
#pragma comment(lib, "Winhttp.lib")

namespace   {
    wstring Extract_WinHttpHeader_ (HINTERNET hRequest, DWORD dwInfoLevel, LPCWSTR pwszName, LPDWORD lpdwIndex)
    {
        DWORD   size    =   0;
        (void)::WinHttpQueryHeaders (hRequest, dwInfoLevel, pwszName, WINHTTP_NO_OUTPUT_BUFFER, &size, lpdwIndex);
        DWORD   error   =   GetLastError ();
        if (error == ERROR_INSUFFICIENT_BUFFER) {
            SmallStackBuffer<wchar_t>   buf (size + 1);
            memset (buf, 0, buf.GetSize ());
            ThrowIfFalseGetLastError (::WinHttpQueryHeaders (hRequest, dwInfoLevel, pwszName, buf, &size, lpdwIndex));
            return buf.begin ();
        }
        else {
            Execution::Platform::Windows::Exception::DoThrow (error);
            return wstring ();
        }
    }
}
#endif







#if     qHasFeature_WinHTTP
/*
 ********************************************************************************
 ***************** Transfer::Connection_WinHTTP::Rep_ ***************************
 ********************************************************************************
 */
Connection_WinHTTP::Rep_::Rep_ ()
    : fSessionHandle_ (nullptr)
    , fConnectionHandle_ (nullptr)
{
}

Connection_WinHTTP::Rep_::~Rep_ ()
{
    if (fConnectionHandle_ != nullptr) {
        Verify (::WinHttpCloseHandle (fConnectionHandle_));
    }
    if (fSessionHandle_ != nullptr) {
        Verify (::WinHttpCloseHandle (fSessionHandle_));
    }
}

DurationSecondsType Connection_WinHTTP::Rep_::GetTimeout () const override
{
    AssertNotImplemented ();
    return 0;
}

void    Connection_WinHTTP::Rep_::SetTimeout (DurationSecondsType timeout) override
{
    AssertNotImplemented ();
}

URL     Connection_WinHTTP::Rep_::GetURL () const override
{
    // needs work... - not sure this is safe - may need to cache orig... instead of reparsing...
    AssertNotImplemented ();
//  return URL (String::FromUTF8 (fCURLCache_URL_).As<wstring> ());
    return URL ();
}

void    Connection_WinHTTP::Rep_::SetURL (const URL& url) override
{
    AssureHasHandle_ ();
    AssertNotImplemented ();
}

void    Connection_WinHTTP::Rep_::Close ()  override
{
    if (fConnectionHandle_ != nullptr) {
        Verify (::WinHttpCloseHandle (fConnectionHandle_));
        fConnectionHandle_ = nullptr;
    }
    if (fSessionHandle_ != nullptr) {
        Verify (::WinHttpCloseHandle (fSessionHandle_));
        fSessionHandle_ = nullptr;
    }
}

Response    Connection_WinHTTP::Rep_::SendAndRequest (const Request& request)   override
{
    Close ();//tmphack
    Response    response;

    /*
     * Though we could create a DIFFERNT API - that managed a session object - like the WinHTTP session object, for now,
     * just KISS. We DONT cache a single session, because this code could be used by multiple 'users' -
     * when called within HealthFrameWorks - for example.
     *
     */
    String      userAgent = String_Constant (L"Stroika/2.0");
    map<String, String>  useHeadersMap   =   request.fOverrideHeaders;
    {
        // We must have an empty 'accept-encoding' to prvent being sent stuff in gzip/deflate format, which WinHTTP
        // appears to not decode (and neither do I).
        map<String, String>::iterator i  =   useHeadersMap.find (HeaderName::kAcceptEncoding);
        if (i == useHeadersMap.end ()) {
            useHeadersMap.insert (map<String, String>::value_type (HeaderName::kAcceptEncoding, wstring ()));
        }
        else {
            i->second = wstring ();
        }
    }
    {
        map<String, String>::iterator i  =   useHeadersMap.find (HeaderName::kUserAgent);
        if (i != useHeadersMap.end ()) {
            userAgent = i->second;
            useHeadersMap.erase (i);
        }
    }
    String  useHeaderStrBuf;
    {
        for (map<String, String>::const_iterator i = useHeadersMap.begin (); i != useHeadersMap.end (); ++i) {
            useHeaderStrBuf += i->first + L": " + i->second + L"\r\n";
        }
    }
    fSessionHandle_ = ::WinHttpOpen (userAgent.c_str (), WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);


    //URL   crackedURL (url);
    URL crackedURL (L"http://www");
    fConnectionHandle_ = ::WinHttpConnect (fSessionHandle_, crackedURL.fHost.c_str (), crackedURL.GetEffectivePortNumber (), 0);

    bool    useSecureHTTP   =   crackedURL.fProtocol == L"https";

// Lots more work todo to adapt this code...
#if 0
    AutoWinINETHandle   hRequest (
        ::WinHttpOpenRequest (hConnection, method.c_str (), crackedURL.fRelPath.c_str (),
                              nullptr, WINHTTP_NO_REFERER,
                              WINHTTP_DEFAULT_ACCEPT_TYPES,
                              useSecureHTTP ? WINHTTP_FLAG_SECURE : 0
                             )
    );

    bool    sslExceptionProblem =   false;
RetryWithNoCERTCheck:

    const   bool    kAllowBadCerts  =   true;   // may want to make this a param passed in!
    if (kAllowBadCerts and sslExceptionProblem) {
        DWORD          dwOptions   =
            SECURITY_FLAG_IGNORE_CERT_CN_INVALID
            | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID
            | SECURITY_FLAG_IGNORE_UNKNOWN_CA
            | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE
            ;
        Verify (::WinHttpSetOption (hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwOptions, sizeof (dwOptions)));
    }

    ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.35f);
    try {
        string  bodyAsUTF8  =   WideStringToUTF8 (body);
        ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.36f);
        ThrowIfFalseGetLastError (::WinHttpSendRequest (
                                      hRequest,
                                      useHeaderStrBuf.c_str (), 0,
                                      (LPVOID)bodyAsUTF8.c_str (), bodyAsUTF8.size (),
                                      bodyAsUTF8.size (),
                                      NULL
                                  )
                                 );
        ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.36f);

        // this must be called before the 'body' goes out of scope!
        ThrowIfFalseGetLastError (::WinHttpReceiveResponse (hRequest, nullptr));
    }
    catch (const Execution::Platform::Windows::Exception& e) {
        bool    looksLikeSSLError   =   (e == ERROR_WINHTTP_SECURE_FAILURE);
        if (looksLikeSSLError and not sslExceptionProblem) {
            DbgTrace ("Got %d ssl error so retrying with flags to disable cert checking", (DWORD)e);
            sslExceptionProblem = true;
            goto RetryWithNoCERTCheck;
        }
        Execution::DoReThrow ();
    }
    ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.4f);

    list<vector<Byte> > bytesRead;
    unsigned int totalBytes =   0;
    {
        // Keep reading data til all done
        float   percentUpTo =   0.45f;      // fake measuring progress...
        DWORD   dwSize = 0;
        do {
            if (percentUpTo < .9f) {
                percentUpTo += 0.05f;
            }
            if (Time::GetTickCount () > startOfSendAt + timeout) {
                DbgTrace (_T ("throwing Timeout"));
                Execution::DoThrow (Execution::Platform::Windows::Exception (WAIT_TIMEOUT));
            }
            ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, percentUpTo);

            // Check for available data.
            dwSize = 0;
            ThrowIfFalseGetLastError (::WinHttpQueryDataAvailable (hRequest, &dwSize));
            SmallStackBuffer<Byte>  outBuffer (dwSize);
            memset (outBuffer, 0, dwSize);
            DWORD   dwDownloaded    =   0;
            ThrowIfFalseGetLastError (::WinHttpReadData (hRequest, outBuffer, dwSize, &dwDownloaded));
            Assert (dwDownloaded <= dwSize);
            totalBytes += dwDownloaded;
            bytesRead.push_back (vector<Byte> (outBuffer.begin (), outBuffer.begin () + dwDownloaded));
        }
        while (dwSize > 0);
    }

    ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.95f);

    // Here - we must convert the chunks of bytes to a big blob and a string
    // This API assumes the HTTP-result is a string
    //
    // probably shoudl check header content-type for codepage, but this SB OK for now...
    {
        vector<Byte>    bytesArray;
        bytesArray.reserve (totalBytes);
        for (list<vector<Byte> >::const_iterator i = bytesRead.begin (); i != bytesRead.end (); ++i) {
            Containers::Append2Vector (&bytesArray, *i);
        }
        result.fBody = MapUNICODETextWithMaybeBOMTowstring (reinterpret_cast<char*> (Containers::Start (bytesArray)), reinterpret_cast<char*> (Containers::End (bytesArray)));
    }

    {
        wstring statusStr   =       Extract_WinHttpHeader (hRequest, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_HEADER_INDEX);
        wstring statusText  =       Extract_WinHttpHeader (hRequest, WINHTTP_QUERY_STATUS_TEXT, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_HEADER_INDEX);
        int     status      =       _wtoi (statusStr.c_str ());
        DbgTrace (_T ("Status = %d"), status);
        if (not HTTPException::IsHTTPStatusOK (status)) {
            if (WINHTTP_ERROR_BASE <= status and status <= WINHTTP_ERROR_BASE) {
                Execution::DoThrow (Execution::Platform::Windows::HRESULTErrorException (MAKE_HRESULT (SEVERITY_ERROR, FACILITY_INTERNET, status)));
            }
            HTTPException::DoThrowIfError (status, statusText, result.fBody);
        }
    }


    /*
     * We COULD check (and this code does if enabled) check to see if the cert was valid - expired - or whatever,
     * but in an advisory fasion. If we want to provide some kind of optional arg to this function to optionally
     * return that info - we could use this. BUT - we need to manually figure out if its expired or
     * whatever.
     */
    if (useSecureHTTP) {
        WINHTTP_CERTIFICATE_INFO   certInfo;
        memset (&certInfo, 0, sizeof (certInfo));
        DWORD          dwCertInfoSize = sizeof (certInfo);
        certInfo.dwKeySize = sizeof (certInfo);
        ThrowIfFalseGetLastError (::WinHttpQueryOption (hRequest, WINHTTP_OPTION_SECURITY_CERTIFICATE_STRUCT, &certInfo, &dwCertInfoSize));

        result.fSSLInfo.fValidationStatus = sslExceptionProblem ?
                                            SSLResultInfo::eSSLFailure :
                                            SSLResultInfo::eSSLOK
                                            ;
        if (certInfo.lpszSubjectInfo != nullptr) {
            wstring subject =   certInfo.lpszSubjectInfo;
            result.fSSLInfo.fSubjectCommonName = subject;
            {
                size_t i = subject.find ('\r');
                if (i != wstring::npos) {
                    result.fSSLInfo.fSubjectCommonName = result.fSSLInfo.fSubjectCommonName.substr (0, i);
                }
            }
        }
        if (certInfo.lpszIssuerInfo != nullptr) {
            result.fSSLInfo.fIssuer = certInfo.lpszIssuerInfo;
        }
        // check dates
        Date    startCertDate   =   DateTime (certInfo.ftStart).GetDate ();
        Date    endCertDate     =   DateTime (certInfo.ftExpiry).GetDate ();
        Date    now             =   DateTime::GetToday ();
        if (now < startCertDate) {
            result.fSSLInfo.fValidationStatus = SSLResultInfo::eCertNotYetValid;
        }
        else if (endCertDate < now) {
            result.fSSLInfo.fValidationStatus = SSLResultInfo::eCertExpired;
        }

        if (not Characters::StringsCIEqual (crackedURL.fHost, result.fSSLInfo.fSubjectCommonName) and
                not Characters::StringsCIEqual (crackedURL.fHost, L"www." + result.fSSLInfo.fSubjectCommonName)
           ) {
            result.fSSLInfo.fValidationStatus = SSLResultInfo::eHostnameMismatch;
        }
        if (certInfo.lpszSubjectInfo != nullptr) {
            ::LocalFree (certInfo.lpszSubjectInfo);
        }
        if (certInfo.lpszIssuerInfo != nullptr) {
            ::LocalFree (certInfo.lpszIssuerInfo);
        }
        if (certInfo.lpszEncryptionAlgName != nullptr) {
            ::LocalFree (certInfo.lpszEncryptionAlgName);
        }
        if (certInfo.lpszProtocolName != nullptr) {
            ::LocalFree (certInfo.lpszProtocolName);
        }
        if (certInfo.lpszSignatureAlgName != nullptr) {
            ::LocalFree (certInfo.lpszSignatureAlgName);
        }
    }


    // copy/fill in result.fHeaders....
    {
        wstring rr  =   Extract_WinHttpHeader (hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_HEADER_INDEX);
        // now break into lines
        for (size_t i = 0; i < rr.length ();) {
            size_t  endOfRegion =   rr.find_first_of (L"\r\n", i);
            if (endOfRegion == wstring::npos) {
                endOfRegion = rr.length ();
            }
            wstring thisLine    =   rr.substr (i, endOfRegion - i);
            // now parse thisLine
            if (thisLine.length () > 5) {   // apx min useful length - not real magic here - just optimization - avoid blank lines (cuz of how we parse crlf)
                // stuff  before the colon is the key, and stuff after the colon (both trimmed) is the value
                size_t  colonI  =   thisLine.find (':');
                if (colonI != wstring::npos) {
                    wstring key =   Characters::Trim (thisLine.substr (0, colonI));
                    wstring val =   Characters::Trim (thisLine.substr (colonI + 1));
                    if (not key.empty ()) {
                        result.fHeaders.insert (map<wstring, wstring>::value_type (key, val));
                    }
                }
            }
            i = endOfRegion + 1;
        }
    }
#endif

    return response;
}

void    Connection_WinHTTP::Rep_::AssureHasHandle_ ()
{
}
#endif





#if     qHasFeature_WinHTTP
/*
 ********************************************************************************
 ********************** Transfer::Connection_WinHTTP ****************************
 ********************************************************************************
 */
Connection_WinHTTP::Connection_WinHTTP ()
    : Connection (shared_ptr<_IRep> (DEBUG_NEW Rep_ ()))
{
}
#endif

