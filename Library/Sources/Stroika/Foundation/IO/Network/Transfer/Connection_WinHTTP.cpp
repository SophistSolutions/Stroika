/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <optional>

#if qHasFeature_WinHTTP
#include <windows.h>

#include <Winhttp.h>
#endif

#include <list>

#include "../../../Characters/CString/Utilities.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Characters/ToString.h"
#include "../../../Containers/STL/Utilities.h"
#include "../../../Execution/Finally.h"
#include "../../../Execution/Throw.h"
#include "../../../Execution/TimeOutException.h"
#include "../../../Time/Date.h"
#include "../../../Time/DateTime.h"
#if qPlatform_Windows
#include "../../../Execution/Platform/Windows/Exception.h"
#include "../../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../HTTP/Exception.h"
#include "../HTTP/Headers.h"

#include "Connection_WinHTTP.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;

#if qPlatform_Windows
using Stroika::Foundation::Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif
using Stroika::Foundation::Memory::StackBuffer;

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::IO::Network::Transfer, qHasFeature_WinHTTP, qHasFeature_WinHTTP);

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 *  TODO:
 *
 */

#if qHasFeature_WinHTTP
// otherwise modules linking with this code will tend to get link errors without explicitly linking
// to this module...
#pragma comment(lib, "Winhttp.lib")
#endif

#if qHasFeature_WinHTTP
namespace {
    struct AutoWinHINTERNET_ {
        HINTERNET fHandle;
        explicit AutoWinHINTERNET_ (HINTERNET handle)
            : fHandle{handle}
        {
            ThrowIfZeroGetLastError (fHandle);
        }
        AutoWinHINTERNET_ ()                         = delete;
        AutoWinHINTERNET_ (const AutoWinHINTERNET_&) = delete;
        ~AutoWinHINTERNET_ ()
        {
            Verify (::WinHttpCloseHandle (fHandle));
        }
        operator HINTERNET ()
        {
            return fHandle;
        }
        nonvirtual const AutoWinHINTERNET_& operator= (const AutoWinHINTERNET_&) = delete;
    };
}
#endif

#if qHasFeature_WinHTTP
class Connection_WinHTTP::Rep_ : public Connection::IRep {
public:
    Rep_ (const Connection::Options& options)
        : fOptions_{options}
    {
    }
    Rep_ (const Rep_&) = delete;
    virtual ~Rep_ ();

public:
    nonvirtual Rep_& operator= (const Rep_&) = delete;

public:
    virtual Options GetOptions () const override
    {
        return fOptions_;
    }
    virtual DurationSecondsType GetTimeout () const override;
    virtual void                SetTimeout (DurationSecondsType timeout) override;
    virtual URI                 GetSchemeAndAuthority () const override;
    virtual void                SetSchemeAndAuthority (const URI& schemeAndAuthority) override;
    virtual void                Close () override;
    virtual Response            Send (const Request& request) override;

private:
    nonvirtual void SetAuthorityRelativeURL_ (const URI& url);

private:
    nonvirtual void AssureHasSessionHandle_ (const String& userAgent);
    nonvirtual void AssureHasConnectionHandle_ ();

private:
    Connection::Options           fOptions_;
    DurationSecondsType           fTimeout_{Time::kInfinite};
    URI                           fURL_;
    shared_ptr<AutoWinHINTERNET_> fSessionHandle_;
    String                        fSessionHandle_UserAgent_;
    shared_ptr<AutoWinHINTERNET_> fConnectionHandle_;
};
#endif

#if qHasFeature_WinHTTP
namespace {
    wstring Extract_WinHttpHeader_ (HINTERNET hRequest, DWORD dwInfoLevel, LPCWSTR pwszName, LPDWORD lpdwIndex)
    {
        DWORD size = 0;
        (void)::WinHttpQueryHeaders (hRequest, dwInfoLevel, pwszName, WINHTTP_NO_OUTPUT_BUFFER, &size, lpdwIndex);
        DWORD error = GetLastError ();
        if (error == ERROR_INSUFFICIENT_BUFFER) {
            StackBuffer<wchar_t> buf{Memory::eUninitialized, size + 1};
            (void)::memset (buf, 0, buf.GetSize ());
            ThrowIfZeroGetLastError (::WinHttpQueryHeaders (hRequest, dwInfoLevel, pwszName, buf, &size, lpdwIndex));
            return buf.begin ();
        }
        else {
            Execution::ThrowSystemErrNo (error);
            return wstring{};
        }
    }
}
#endif

#if qHasFeature_WinHTTP
/*
 ********************************************************************************
 ***************** Transfer::Connection_WinHTTP::Rep_ ***************************
 ********************************************************************************
 */
Connection_WinHTTP::Rep_::~Rep_ ()
{
    fConnectionHandle_.reset ();
    fSessionHandle_.reset ();
}

DurationSecondsType Connection_WinHTTP::Rep_::GetTimeout () const
{
    return fTimeout_;
}

void Connection_WinHTTP::Rep_::SetTimeout (DurationSecondsType timeout)
{
    fTimeout_ = timeout; // affects subsequent calls to send...
}

void Connection_WinHTTP::Rep_::SetAuthorityRelativeURL_ (const URI& url)
{
    URI newURL = url; // almost but not quite the same as fURL_.Combine (url)
    newURL.SetScheme (fURL_.GetScheme ());
    newURL.SetAuthority (fURL_.GetAuthority ());
    if (fURL_ != url) {
        fConnectionHandle_.reset ();
        fURL_ = newURL;
    }
}

URI Connection_WinHTTP::Rep_::GetSchemeAndAuthority () const
{
    return fURL_.GetSchemeAndAuthority ();
}

void Connection_WinHTTP::Rep_::SetSchemeAndAuthority (const URI& schemeAndAuthority)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Connection_WinHTTP::Rep_::SetSchemeAndAuthority ('%s')", Characters::ToString (schemeAndAuthority).c_str ());
#endif
    URI newURL = fURL_;
    newURL.SetScheme (schemeAndAuthority.GetScheme ());
    newURL.SetAuthority (schemeAndAuthority.GetAuthority ());
    if (fURL_ != newURL) {
        fConnectionHandle_.reset ();
        fURL_ = newURL;
    }
}

void Connection_WinHTTP::Rep_::Close ()
{
    fConnectionHandle_.reset ();
    fSessionHandle_.reset ();
}

Response Connection_WinHTTP::Rep_::Send (const Request& request)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Connection_WinHTTP::Rep_::Send", L"request=%s", Characters::ToString (request).c_str ())};
#endif
    Request useRequest = request;

    SetAuthorityRelativeURL_ (useRequest.fAuthorityRelativeURL);

    BLOB                              data; // usually empty, but provided for some methods like POST
    Mapping<String, String>           headers;
    HTTP::Status                      status{};
    optional<Response::SSLResultInfo> serverEndpointSSLInfo;

    Time::DurationSecondsType startOfSendAt = Time::GetTickCount ();
    Time::DurationSecondsType endBy         = fTimeout_ == Time::kInfinite ? Time::kInfinite : (startOfSendAt + fTimeout_);

    /*
     * Though we could create a DIFFERENT API - that managed a session object - like the WinHTTP session object, for now,
     * just KISS. We DON'T cache a single session, because this code could be used by multiple 'users' -
     * when called within HealthFrameWorks - for example.
     *
     */
    String                  userAgent     = fOptions_.fUserAgent;
    Mapping<String, String> useHeadersMap = useRequest.fOverrideHeaders;
    {
        // We must have an empty 'accept-encoding' to prevent being sent stuff in gzip/deflate format, which WinHTTP
        // appears to not decode (and neither do I).
        useHeadersMap.Add (String_Constant{HeaderName::kAcceptEncoding}, String{});
    }
    Cache::EvalContext cacheContext;
    if (fOptions_.fCache != nullptr) {
        if (auto r = fOptions_.fCache->OnBeforeFetch (&cacheContext, fURL_.GetSchemeAndAuthority (), &useRequest)) {
            // shortcut - we already have a cached answer
            return *r;
        }
    }
    {
        if (useHeadersMap.Lookup (String_Constant{HeaderName::kUserAgent}, &userAgent)) {
            useHeadersMap.Remove (String_Constant{HeaderName::kUserAgent});
        }
    }
    if (fOptions_.fAuthentication and fOptions_.fAuthentication->GetOptions () == Connection::Options::Authentication::Options::eProactivelySendAuthentication) {
        useHeadersMap.Add (String_Constant{HeaderName::kAuthorization}, fOptions_.fAuthentication->GetAuthToken ());
    }
    String useHeaderStrBuf;
    {
        for (auto i = useHeadersMap.begin (); i != useHeadersMap.end (); ++i) {
            useHeaderStrBuf += i->fKey + L": "sv + i->fValue + L"\r\n"sv;
        }
    }

    AssureHasSessionHandle_ (userAgent);
    Assert (fSessionHandle_ != nullptr);
    AssureHasConnectionHandle_ ();
    Assert (fConnectionHandle_ != nullptr);

    if (fOptions_.fTCPKeepAlives) {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/aa384066(v=vs.85).aspx - MSFT says must be > 30 seconds, and cannot be disabled, so just set to long timeout before sending keepalive
        // MSFT docs appear to indicate this wont work wtih a handle so I'm not really sure how to use/or if to use
        //DWORD dwOptionsTimeout = fOptions_.fTCPKeepAlives->fEnabled ? 30000 : 1000 * 1000;
        //Verify (::WinHttpSetOption (*fConnectionHandle_, WINHTTP_OPTION_WEB_SOCKET_KEEPALIVE_INTERVAL, &dwOptionsTimeout, sizeof (dwOptionsTimeout)));
    }

    bool useSecureHTTP = fURL_.GetScheme () and fURL_.GetScheme ()->IsSecure ();

    AutoWinHINTERNET_ hRequest (
        ::WinHttpOpenRequest (*fConnectionHandle_, useRequest.fMethod.c_str (), fURL_.GetAuthorityRelativeResource ().c_str (),
                              nullptr, WINHTTP_NO_REFERER,
                              WINHTTP_DEFAULT_ACCEPT_TYPES,
                              useSecureHTTP ? WINHTTP_FLAG_SECURE : 0));

    // See https://stroika.atlassian.net/browse/STK-442 - we pre-set to avoid double try on failure, but
    // we cannot IF we want to know if SSL connect failed (until I figure out how)
    constexpr bool kDefault_FailConnectionIfSSLCertificateInvalid{true};
    if (not fOptions_.fReturnSSLInfo and not fOptions_.fFailConnectionIfSSLCertificateInvalid.value_or (kDefault_FailConnectionIfSSLCertificateInvalid)) {
        DWORD dwOptions =
            SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
        Verify (::WinHttpSetOption (hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwOptions, sizeof (dwOptions)));
    }
    if (not fOptions_.fSupportSessionCookies) {
        /*
         *  From https://msdn.microsoft.com/en-us/library/windows/desktop/aa384066(v=vs.85).aspx
         *      Be aware that this feature should only be passed to WinHttpSetOption on request handles after the request handle is created with WinHttpOpenRequest, and before the request is sent with WinHttpSendRequest.
         */
        DWORD dwOptions = WINHTTP_DISABLE_COOKIES;
        Verify (::WinHttpSetOption (hRequest, WINHTTP_OPTION_DISABLE_FEATURE, &dwOptions, sizeof (dwOptions)));
    }

    bool got401 = false; // if we get one, we add credentials, but if we get two, its time to give up

    bool sslExceptionProblem = false;
RetryWithNoCERTCheck:

    //
    // REALLY - don't want these flags here - but have a CALLBACK whcih checks arbitrary rules and THROWS if unhappy - and doesn't do rest of fetch...
    //      TODO!!!
    //
    // See https://stroika.atlassian.net/browse/STK-442
    //
    if (fOptions_.fReturnSSLInfo and not fOptions_.fFailConnectionIfSSLCertificateInvalid.value_or (kDefault_FailConnectionIfSSLCertificateInvalid) and sslExceptionProblem) {
        DWORD dwOptions =
            SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
        Verify (::WinHttpSetOption (hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwOptions, sizeof (dwOptions)));
    }

RetryWithAuth:
    try {
        if (useRequest.fData.size () > numeric_limits<DWORD>::max ()) {
            Throw (Execution::Exception{L"Too large a message to send using WinHTTP"sv});
        }
        ThrowIfZeroGetLastError (::WinHttpSendRequest (
            hRequest,
            useHeaderStrBuf.c_str (), static_cast<DWORD> (-1),
            useRequest.fData.empty () ? nullptr : const_cast<byte*> (useRequest.fData.begin ()), static_cast<DWORD> (useRequest.fData.size ()),
            static_cast<DWORD> (useRequest.fData.size ()),
            NULL));

        // this must be called before the 'body' goes out of scope!
        ThrowIfZeroGetLastError (::WinHttpReceiveResponse (hRequest, nullptr));
    }
    catch (const system_error& e) {
        if (fOptions_.fReturnSSLInfo) {
            bool looksLikeSSLError = (e.code () == error_code (ERROR_WINHTTP_SECURE_FAILURE, system_category ()));
            if (looksLikeSSLError and not sslExceptionProblem) {
                DbgTrace ("Got %d ssl error so retrying with flags to disable cert checking", e.code ().value ());
                sslExceptionProblem = true;
                goto RetryWithNoCERTCheck;
            }
        }
        Execution::ReThrow ();
    }

    list<vector<byte>> bytesRead;
    unsigned int       totalBytes = 0;
    {
        // Keep reading data til all done
        DWORD dwSize = 0;
        do {
            if (Time::GetTickCount () > endBy) [[UNLIKELY_ATTR]] {
                Execution::Throw (Execution::TimeOutException::kThe);
            }

            // Check for available data.
            dwSize = 0;
            ThrowIfZeroGetLastError (::WinHttpQueryDataAvailable (hRequest, &dwSize));
            StackBuffer<byte> outBuffer{Memory::eUninitialized, dwSize};
            memset (outBuffer, 0, dwSize);
            DWORD dwDownloaded = 0;
            ThrowIfZeroGetLastError (::WinHttpReadData (hRequest, outBuffer, dwSize, &dwDownloaded));
            Assert (dwDownloaded <= dwSize);
            totalBytes += dwDownloaded;
            bytesRead.push_back (vector<byte>{outBuffer.begin (), outBuffer.begin () + dwDownloaded});
        } while (dwSize > 0);
    }

    // Here - we must convert the chunks of bytes to a big blob and a string
    // This API assumes the HTTP-result is a string
    //
    // probably should check header content-type for codepage, but this SB OK for now...
    {
        StackBuffer<byte> bytesArray{Memory::eUninitialized, totalBytes};
        size_t            iii = 0;
        for (auto i = bytesRead.begin (); i != bytesRead.end (); ++i) {
            auto v2 = *i;
            for (auto ii = v2.begin (); ii != v2.end (); ++ii) {
                bytesArray[iii] = *ii;
                ++iii;
            }
        }
        Assert (iii == totalBytes);
        data = BLOB{bytesArray.begin (), bytesArray.end ()};
    }

    // don't throw here - record the bad status in the response. The reason is we often wish to read the whole body of the response.
    // It can contain an explanation of the error (such as soap fault) more detailed than the status line response
    {
        wstring statusStr  = Extract_WinHttpHeader_ (hRequest, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_HEADER_INDEX);
        wstring statusText = Extract_WinHttpHeader_ (hRequest, WINHTTP_QUERY_STATUS_TEXT, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_HEADER_INDEX);
        status             = static_cast<HTTP::Status> (_wtoi (statusStr.c_str ()));
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (_T ("Status = %d"), status);
#endif
    }

    if (status == 401 and not got401 and fOptions_.fAuthentication and fOptions_.fAuthentication->GetOptions () == Connection::Options::Authentication::Options::eRespondToWWWAuthenticate) {
        got401 = true;
        DWORD dwSupportedSchemes{};
        DWORD dwFirstScheme{};
        DWORD dwTarget{};
        if (::WinHttpQueryAuthSchemes (hRequest, &dwSupportedSchemes, &dwFirstScheme, &dwTarget)) {
            auto chooseAuthScheme = [] (DWORD supportedSchemes) -> DWORD {
                // see https://msdn.microsoft.com/en-us/library/windows/desktop/aa383144(v=vs.85).aspx
                // ChooseAuthScheme
                if (supportedSchemes & WINHTTP_AUTH_SCHEME_NEGOTIATE)
                    return WINHTTP_AUTH_SCHEME_NEGOTIATE;
                else if (supportedSchemes & WINHTTP_AUTH_SCHEME_NTLM)
                    return WINHTTP_AUTH_SCHEME_NTLM;
                else if (supportedSchemes & WINHTTP_AUTH_SCHEME_PASSPORT)
                    return WINHTTP_AUTH_SCHEME_PASSPORT;
                else if (supportedSchemes & WINHTTP_AUTH_SCHEME_DIGEST)
                    return WINHTTP_AUTH_SCHEME_DIGEST;
                else if (supportedSchemes & WINHTTP_AUTH_SCHEME_BASIC)
                    return WINHTTP_AUTH_SCHEME_BASIC;
                else
                    return 0;
            };
            DWORD dwSelectedScheme = chooseAuthScheme (dwSupportedSchemes);
            if (dwSelectedScheme != 0) {
                auto nameAndPassword = *fOptions_.fAuthentication->GetUsernameAndPassword (); // if eRespondToWWWAuthenticate we must have username/password (Options CTOR requirement)
                Verify (::WinHttpSetCredentials (hRequest, dwTarget, dwSelectedScheme, nameAndPassword.first.AsSDKString ().c_str (), nameAndPassword.second.AsSDKString ().c_str (), nullptr));
                goto RetryWithAuth;
            }
        }
    }

    /*
     * We COULD check (and this code does if enabled) check to see if the cert was valid - expired - or whatever,
     * but in an advisory fasion. If we want to provide some kind of optional arg to this function to optionally
     * return that info - we could use this. BUT - we need to manually figure out if its expired or
     * whatever.
     */
    if (useSecureHTTP and fOptions_.fReturnSSLInfo) {
        WINHTTP_CERTIFICATE_INFO certInfo{};
        DWORD                    dwCertInfoSize = sizeof (certInfo);
        certInfo.dwKeySize                      = sizeof (certInfo);
        ThrowIfZeroGetLastError (::WinHttpQueryOption (hRequest, WINHTTP_OPTION_SECURITY_CERTIFICATE_STRUCT, &certInfo, &dwCertInfoSize));
        [[maybe_unused]] auto&& cleanup = Execution::Finally (
            [certInfo] () noexcept {
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
            });

        Response::SSLResultInfo resultSSLInfo;
        resultSSLInfo.fValidationStatus = sslExceptionProblem ? Response::SSLResultInfo::ValidationStatus::eSSLFailure : Response::SSLResultInfo::ValidationStatus::eSSLOK;
        if (certInfo.lpszSubjectInfo != nullptr) {
            wstring subject                  = certInfo.lpszSubjectInfo;
            resultSSLInfo.fSubjectCommonName = subject;
            {
                size_t i = subject.find ('\r');
                if (i != wstring::npos) {
                    resultSSLInfo.fSubjectCommonName = resultSSLInfo.fSubjectCommonName.substr (0, i);
                }
            }
        }
        if (certInfo.lpszIssuerInfo != nullptr) {
            resultSSLInfo.fIssuer = certInfo.lpszIssuerInfo;
        }
        // check dates
        Date startCertDate = DateTime{certInfo.ftStart}.GetDate ();
        Date endCertDate   = DateTime{certInfo.ftExpiry}.GetDate ();
        Date now           = DateTime::GetToday ();
        if (now < startCertDate) {
            resultSSLInfo.fValidationStatus = Response::SSLResultInfo::ValidationStatus::eCertNotYetValid;
        }
        else if (endCertDate < now) {
            resultSSLInfo.fValidationStatus = Response::SSLResultInfo::ValidationStatus::eCertExpired;
        }

        if (not fURL_.GetAuthority () or not fURL_.GetAuthority ()->GetHost () or not fURL_.GetAuthority ()->GetHost ()->AsRegisteredName ()) {
            Execution::Throw (Execution::RuntimeErrorException{L"Cannot validate TLS without a host name"sv});
        }
        auto equalsComparer = String::EqualsComparer{CompareOptions::eCaseInsensitive};
        if (not equalsComparer (*fURL_.GetAuthority ()->GetHost ()->AsRegisteredName (), resultSSLInfo.fSubjectCommonName) and
            not equalsComparer (*fURL_.GetAuthority ()->GetHost ()->AsRegisteredName (), L"www." + resultSSLInfo.fSubjectCommonName)) {
            resultSSLInfo.fValidationStatus = Response::SSLResultInfo::ValidationStatus::eHostnameMismatch;
        }

        serverEndpointSSLInfo = resultSSLInfo;
    }

    // copy/fill in result.fHeaders....
    {
        wstring rr = Extract_WinHttpHeader_ (hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_HEADER_INDEX);
        // now break into lines
        for (size_t i = 0; i < rr.length ();) {
            size_t endOfRegion = rr.find_first_of (L"\r\n", i);
            if (endOfRegion == wstring::npos) {
                endOfRegion = rr.length ();
            }
            wstring thisLine = rr.substr (i, endOfRegion - i);
            // now parse thisLine
            if (thisLine.length () > 5) { // apx min useful length - not real magic here - just optimization - avoid blank lines (cuz of how we parse crlf)
                // stuff  before the colon is the key, and stuff after the colon (both trimmed) is the value
                size_t colonI = thisLine.find (':');
                if (colonI != wstring::npos) {
                    wstring key = Characters::CString::Trim (thisLine.substr (0, colonI));
                    wstring val = Characters::CString::Trim (thisLine.substr (colonI + 1));
                    if (not key.empty ()) {
                        headers.Add (key, val);
                    }
                }
            }
            i = endOfRegion + 1;
        }
    }

    Response result{data, status, headers, serverEndpointSSLInfo};
    if (fOptions_.fCache != nullptr) {
        fOptions_.fCache->OnAfterFetch (cacheContext, &result);
    }

    return result;
}

void Connection_WinHTTP::Rep_::AssureHasSessionHandle_ (const String& userAgent)
{
    if (fSessionHandle_UserAgent_ != userAgent) {
        fConnectionHandle_.reset ();
        fSessionHandle_.reset ();
    }
    if (fSessionHandle_ == nullptr) {
        fSessionHandle_           = make_shared<AutoWinHINTERNET_> (::WinHttpOpen (userAgent.c_str (), WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
        fSessionHandle_UserAgent_ = userAgent;
        if (fOptions_.fMaxAutomaticRedirects == 0) {
            DWORD dwOptions = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
            Verify (::WinHttpSetOption (*fSessionHandle_, WINHTTP_OPTION_REDIRECT_POLICY, &dwOptions, sizeof (dwOptions)));
        }
        else {
            DWORD dwOptions = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP; // sb WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS?
            Verify (::WinHttpSetOption (*fSessionHandle_, WINHTTP_OPTION_REDIRECT_POLICY, &dwOptions, sizeof (dwOptions)));
            // According to https://msdn.microsoft.com/en-us/library/windows/desktop/aa384066%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396,
            // WINHTTP_OPTION_MAX_HTTP_AUTOMATIC_REDIRECTS is obsolete
        }
    }
}

void Connection_WinHTTP::Rep_::AssureHasConnectionHandle_ ()
{
    RequireNotNull (fSessionHandle_);
    if (fConnectionHandle_ == nullptr) {
        if (not fURL_.GetAuthority () or not fURL_.GetAuthority ()->GetHost ()) {
            Execution::Throw (Execution::RuntimeErrorException{L"Cannot connect without a host"sv});
        }
        // NOT SURE - for IPv6 address - if we want to pass encoded value here?
        fConnectionHandle_ = make_shared<AutoWinHINTERNET_> (::WinHttpConnect (*fSessionHandle_, fURL_.GetAuthority ()->GetHost ()->AsEncoded ().c_str (), fURL_.GetPortValue (), 0));
    }
}
#endif

#if qHasFeature_WinHTTP
/*
 ********************************************************************************
 ********************** Transfer::Connection_WinHTTP ****************************
 ********************************************************************************
 */
Connection::Ptr Connection_WinHTTP::New (const Options& options)
{
    return Connection::Ptr{make_shared<Rep_> (options)};
}
#endif
