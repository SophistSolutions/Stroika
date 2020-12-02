/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <csignal>

#if qHasFeature_LibCurl
#include <curl/curl.h>
#endif

#include "../../../Characters/Format.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Common/Immortalize.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/Throw.h"

#include "../HTTP/Headers.h"
#include "../HTTP/Methods.h"

#include "Connection_libcurl.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::IO::Network::Transfer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

// Uncomment this line to enable libcurl to print diagnostics to stderr
//#define       USE_LIBCURL_VERBOSE_    1

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::IO::Network::Transfer, qHasFeature_LibCurl, qHasFeature_LibCurl);

#if qHasFeature_LibCurl
namespace {
    inline void AssureLibCurlInitialized_ ()
    {
        static once_flag sOnceFlag_;
        call_once (sOnceFlag_, [] () {
            atexit ([] () {
                ::curl_global_cleanup ();
            });
            /*
             *  @todo review CURL_GLOBAL_SSL
             *
             *   @see http://curl.haxx.se/libcurl/c/curl_global_init.html
             *  not 100% sure what flags to send
             *  CURL_GLOBAL_SSL needed for now, but could interfere with other openssl uses
             */
            Verify (::curl_global_init (CURL_GLOBAL_SSL | CURL_GLOBAL_ACK_EINTR) == 0);
        });
    }
}
#endif

#if qHasFeature_LibCurl
namespace {
    class LibCurl_error_category_ : public error_category {
    public:
        virtual const char* name () const noexcept override
        {
            return "LibCurl error";
        }
        virtual error_condition default_error_condition ([[maybe_unused]] int ev) const noexcept override
        {
            switch (ev) {
                case CURLE_OUT_OF_MEMORY:
                    return errc::not_enough_memory;
                case CURLE_OPERATION_TIMEDOUT:
                    return errc::timed_out;
                case CURLE_LOGIN_DENIED:
                    return errc::permission_denied;
                case CURLE_SEND_ERROR:
                    return errc::io_error;
                case CURLE_RECV_ERROR:
                    return errc::io_error;
            }
            // @todo - not sure how todo this - except by defining new conditions
            //switch (ev) {
            //}
            return error_condition (errc::bad_message); // no idea what to return here
        }
        virtual string message (int ccode) const override
        {
            return ::curl_easy_strerror (static_cast<CURLcode> (ccode));
        }
    };
}

/*
 ********************************************************************************
 ************************ Transfer::LibCurl_error_category **********************
 ********************************************************************************
 */
const std::error_category& Transfer::LibCurl_error_category () noexcept
{
    return Common::Immortalize<LibCurl_error_category_> ();
}
#endif

#if qHasFeature_LibCurl
class Connection_LibCurl::Rep_ : public Connection::IRep {
private:
    Connection::Options fOptions_;

public:
    Rep_ (const Connection::Options& options)
        : fOptions_{options}
    {
        AssureLibCurlInitialized_ ();
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
    nonvirtual void MakeHandleIfNeeded_ ();

private:
    static size_t     s_RequestPayloadReadHandler_ (char* buffer, size_t size, size_t nitems, void* userP);
    nonvirtual size_t RequestPayloadReadHandler_ (byte* buffer, size_t bufSize);

private:
    static size_t     s_ResponseWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP);
    nonvirtual size_t ResponseWriteHandler_ (const byte* ptr, size_t nBytes);

private:
    static size_t     s_ResponseHeaderWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP);
    nonvirtual size_t ResponseHeaderWriteHandler_ (const byte* ptr, size_t nBytes);

private:
    void*                   fCurlHandle_{nullptr};
    URI                     fURL_;
    bool                    fDidCustomMethod_{false};
    vector<byte>            fUploadData_;
    size_t                  fUploadDataCursor_{};
    vector<byte>            fResponseData_;
    Mapping<String, String> fResponseHeaders_;
    curl_slist*             fSavedHeaders_{nullptr};
};
#endif

#if qHasFeature_LibCurl
/*
 ********************************************************************************
 ****************** Transfer::Connection_LibCurl::Rep_ **************************
 ********************************************************************************
 */
Connection_LibCurl::Rep_::~Rep_ ()
{
    if (fCurlHandle_ != nullptr) {
        ::curl_easy_cleanup (fCurlHandle_);
    }
    if (fSavedHeaders_ != nullptr) {
        ::curl_slist_free_all (fSavedHeaders_);
        fSavedHeaders_ = nullptr;
    }
}

DurationSecondsType Connection_LibCurl::Rep_::GetTimeout () const
{
    AssertNotImplemented ();
    return 0;
}

void Connection_LibCurl::Rep_::SetTimeout (DurationSecondsType timeout)
{
    MakeHandleIfNeeded_ ();
    ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_TIMEOUT_MS, static_cast<int> (timeout * 1000)));
    ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_CONNECTTIMEOUT_MS, static_cast<int> (timeout * 1000)));
}

URI Connection_LibCurl::Rep_::GetSchemeAndAuthority () const
{
    return fURL_.GetSchemeAndAuthority ();
}

void Connection_LibCurl::Rep_::SetSchemeAndAuthority (const URI& schemeAndAuthority)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Connection_LibCurl::Rep_::SetSchemeAndAuthority ('%s')", Characters::ToString (schemeAndAuthority).c_str ());
#endif
    fURL_.SetScheme (schemeAndAuthority.GetScheme ());
    fURL_.SetAuthority (schemeAndAuthority.GetAuthority ());
    if (fCurlHandle_ != nullptr) {
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_URL, fURL_.As<string> ().c_str ()));
    }
}

void Connection_LibCurl::Rep_::SetAuthorityRelativeURL_ (const URI& url)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Connection_LibCurl::Rep_::SetAuthorityRelativeURL_ (%s)", Characters::ToString (url).c_str ());
#endif
    URI newURL = url;
    newURL.SetScheme (fURL_.GetScheme ());
    newURL.SetAuthority (fURL_.GetAuthority ());
    if (fCurlHandle_ != nullptr) {
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_URL, newURL.As<string> ().c_str ()));
    }
    fURL_ = newURL;
}

void Connection_LibCurl::Rep_::Close ()
{
    if (fCurlHandle_ != nullptr) {
        ::curl_easy_cleanup (fCurlHandle_);
        fCurlHandle_ = nullptr;
    }
}

size_t Connection_LibCurl::Rep_::s_RequestPayloadReadHandler_ (char* buffer, size_t size, size_t nitems, void* userP)
{
    return reinterpret_cast<Rep_*> (userP)->RequestPayloadReadHandler_ (reinterpret_cast<byte*> (buffer), size * nitems);
}

size_t Connection_LibCurl::Rep_::RequestPayloadReadHandler_ (byte* buffer, size_t bufSize)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Connection_LibCurl::Rep_::RequestPayloadReadHandler_"};
#endif
    size_t bytes2Copy = fUploadData_.size () - fUploadDataCursor_;
    bytes2Copy        = min (bytes2Copy, bufSize);
    ::memcpy (buffer, Traversal::Iterator2Pointer (begin (fUploadData_)) + fUploadDataCursor_, bytes2Copy);
    fUploadDataCursor_ += bytes2Copy;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"bufSize = %d, bytes2Copy=%d", bufSize, bytes2Copy);
#endif
    return bytes2Copy;
}

size_t Connection_LibCurl::Rep_::s_ResponseWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP)
{
#if qStroika_FeatureSupported_Valgrind
    VALGRIND_MAKE_MEM_DEFINED (ptr, size * nmemb); // Handle OpenSSL if not built with purify
#endif
    return reinterpret_cast<Rep_*> (userP)->ResponseWriteHandler_ (reinterpret_cast<const byte*> (ptr), size * nmemb);
}

size_t Connection_LibCurl::Rep_::ResponseWriteHandler_ (const byte* ptr, size_t nBytes)
{
    fResponseData_.insert (fResponseData_.end (), ptr, ptr + nBytes);
    return nBytes;
}

size_t Connection_LibCurl::Rep_::s_ResponseHeaderWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP)
{
#if qStroika_FeatureSupported_Valgrind
    VALGRIND_MAKE_MEM_DEFINED (ptr, size * nmemb); // Handle OpenSSL if not built with purify
#endif
    return reinterpret_cast<Rep_*> (userP)->ResponseHeaderWriteHandler_ (reinterpret_cast<const byte*> (ptr), size * nmemb);
}

size_t Connection_LibCurl::Rep_::ResponseHeaderWriteHandler_ (const byte* ptr, size_t nBytes)
{
    String from = String::FromUTF8 (reinterpret_cast<const char*> (ptr), reinterpret_cast<const char*> (ptr) + nBytes);
    String to;
    size_t i = from.find (':');
    if (i != string::npos) {
        to   = from.SubString (i + 1);
        from = from.SubString (0, i);
    }
    from = from.Trim ();
    to   = to.Trim ();
    fResponseHeaders_.Add (from, to);
    return nBytes;
}

Response Connection_LibCurl::Rep_::Send (const Request& request)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"Connection_LibCurl::Rep_::Send", L"method='%s'", request.fMethod.c_str ()};
#endif
    Request useRequest = request;

    SetAuthorityRelativeURL_ (useRequest.fAuthorityRelativeURL);

    Cache::EvalContext cacheContext;
    if (fOptions_.fCache != nullptr) {
        if (auto r = fOptions_.fCache->OnBeforeFetch (&cacheContext, fURL_.GetSchemeAndAuthority (), &useRequest)) {
            // shortcut - we already have a cached answer
            return *r;
        }
    }

    MakeHandleIfNeeded_ ();
    fUploadData_       = useRequest.fData.As<vector<byte>> ();
    fUploadDataCursor_ = 0;
    fResponseData_.clear ();
    fResponseHeaders_.clear ();

    ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_USERAGENT, fOptions_.fUserAgent.AsUTF8 ().c_str ()));

    if (fOptions_.fSupportSessionCookies) {
        //@todo horrible kludge, but I couldnt find a better way. Will need to use soemthing else for windows, probably!
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_COOKIEFILE, "/dev/null"));
    }

    Mapping<String, String> overrideHeaders = useRequest.fOverrideHeaders;
    if (fOptions_.fAssumeLowestCommonDenominatorHTTPServer) {
        static const Mapping<String, String> kSilenceTheseHeaders_{
            {pair<String, String>{L"Expect"sv, {}},
             pair<String, String>{L"Transfer-Encoding"sv, {}}}};
        overrideHeaders = kSilenceTheseHeaders_ + overrideHeaders;
    }
    if (fOptions_.fAuthentication and fOptions_.fAuthentication->GetOptions () == Connection::Options::Authentication::Options::eProactivelySendAuthentication) {
        overrideHeaders.Add (String_Constant{HeaderName::kAuthorization}, fOptions_.fAuthentication->GetAuthToken ());
    }
    {
        constexpr bool kDefault_FailConnectionIfSSLCertificateInvalid{false};
        // ignore error if compiled without ssl
        (void)::curl_easy_setopt (fCurlHandle_, CURLOPT_SSL_VERIFYPEER, fOptions_.fFailConnectionIfSSLCertificateInvalid.value_or (kDefault_FailConnectionIfSSLCertificateInvalid) ? 1L : 0L);
        (void)::curl_easy_setopt (fCurlHandle_, CURLOPT_SSL_VERIFYHOST, fOptions_.fFailConnectionIfSSLCertificateInvalid.value_or (kDefault_FailConnectionIfSSLCertificateInvalid) ? 2L : 0L);
    }

    if (useRequest.fMethod == HTTP::Methods::kGet) {
        if (fDidCustomMethod_) {
            ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_CUSTOMREQUEST, nullptr));
            fDidCustomMethod_ = false;
        }
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_HTTPGET, 1));
    }
    else if (useRequest.fMethod == HTTP::Methods::kPost) {
        if (fDidCustomMethod_) {
            ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_CUSTOMREQUEST, nullptr));
            fDidCustomMethod_ = false;
        }
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_POST, 1));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_POSTFIELDSIZE, fUploadData_.size ()));
    }
    else if (useRequest.fMethod == HTTP::Methods::kPut) {
        if (fDidCustomMethod_) {
            ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_CUSTOMREQUEST, nullptr));
            fDidCustomMethod_ = false;
        }
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_UPLOAD, fUploadData_.empty () ? 0 : 1));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_INFILESIZE, fUploadData_.size ()));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_PUT, 1));
    }
    else {
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_HTTPGET, 0));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_POST, 0));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_PUT, 0));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_CUSTOMREQUEST, useRequest.fMethod.AsUTF8 ().c_str ()));
    }

    if (fOptions_.fAuthentication and fOptions_.fAuthentication->GetOptions () == Connection::Options::Authentication::Options::eRespondToWWWAuthenticate) {
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY)); // tell libcurl we can use "any" auth, which lets the lib pick one, but it also costs one extra round-trip and possibly sending of all the PUT data twice!
        auto nameAndPassword = *fOptions_.fAuthentication->GetUsernameAndPassword ();           // if eRespondToWWWAuthenticate we must have username/password (Options CTOR requirement)
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_USERNAME, nameAndPassword.first.AsUTF8 ().c_str ()));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_PASSWORD, nameAndPassword.second.AsUTF8 ().c_str ()));
    }

    // grab initial headers and do POST/etc based on args in request...
    curl_slist* tmpH = nullptr;
    for (auto i : overrideHeaders) {
        tmpH = ::curl_slist_append (tmpH, (i.fKey + L": "sv + i.fValue).AsUTF8 ().c_str ());
    }
    AssertNotNull (fCurlHandle_);
    ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_HTTPHEADER, tmpH));
    if (fSavedHeaders_ != nullptr) {
        ::curl_slist_free_all (fSavedHeaders_);
        fSavedHeaders_ = nullptr;
    }
    fSavedHeaders_ = tmpH;

    ThrowIfError (::curl_easy_perform (fCurlHandle_));

    long resultCode = 0;
    ThrowIfError (::curl_easy_getinfo (fCurlHandle_, CURLINFO_RESPONSE_CODE, &resultCode));
    Response result{fResponseData_, static_cast<HTTP::Status> (resultCode), fResponseHeaders_};
    if (fOptions_.fCache != nullptr) {
        fOptions_.fCache->OnAfterFetch (cacheContext, &result);
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning status = %d, dataLen = %d", result.GetStatus (), result.GetData ().size ());
#endif
    return result;
}

void Connection_LibCurl::Rep_::MakeHandleIfNeeded_ ()
{
    if (fCurlHandle_ == nullptr) {
        ThrowIfNull (fCurlHandle_ = ::curl_easy_init ());

        /*
         * Now setup COMMON options we ALWAYS set.
         */
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_URL, fURL_.As<string> ().c_str ()));

#if USE_LIBCURL_VERBOSE_
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_VERBOSE, 1));
#endif

        /*
         *  We may want this to be optional? Or use ares, for resolution (may want that anyhow).
         *  But for now, this is most likely to avoid untoward interactions with other libraries (guess)
         *      --LGP 2015-11-12
         */
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_NOSIGNAL, 1));

#if qDebug && qPlatform_POSIX
        {
            struct sigaction oldact;
            (void)::sigaction (SIGPIPE, NULL, &oldact);
            if (oldact.sa_handler == SIG_DFL) {
                DbgTrace (L"Warning: no override of SIGPIPE. This is a risk factor with curl. Often just ignore");
            }
        }
#endif

        if (fOptions_.fMaxAutomaticRedirects == 0) {
            ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_FOLLOWLOCATION, 0L));
        }
        else {
            ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_FOLLOWLOCATION, 1L));
            ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_MAXREDIRS, fOptions_.fMaxAutomaticRedirects));
            // violate dejure standard but follow defacto standard and only senisble behavior
            // https://curl.haxx.se/libcurl/c/CURLOPT_POSTREDIR.html
            ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_POSTREDIR, CURL_REDIR_POST_301)); // could have used CURL_REDIR_POST_ALL?
        }

        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_READFUNCTION, s_RequestPayloadReadHandler_));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_READDATA, this));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_WRITEFUNCTION, s_ResponseWriteHandler_));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_WRITEDATA, this));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_HEADERFUNCTION, s_ResponseHeaderWriteHandler_));
        ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_WRITEHEADER, this));

        if (fOptions_.fTCPKeepAlives) {
            ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_TCP_KEEPALIVE, fOptions_.fTCPKeepAlives->fEnabled));
#if qPlatform_Linux
            if (fOptions_.fTCPKeepAlives->fTimeIdleBeforeSendingKeepalives) {
                ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_TCP_KEEPIDLE, *fOptions_.fTCPKeepAlives->fTimeIdleBeforeSendingKeepalives));
            }
            if (fOptions_.fTCPKeepAlives->fTimeBetweenIndividualKeepaliveProbes) {
                ThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_TCP_KEEPINTVL, *fOptions_.fTCPKeepAlives->fTimeBetweenIndividualKeepaliveProbes));
            }
#endif
        }
    }
}
#endif

#if qHasFeature_LibCurl
/*
 ********************************************************************************
 ********************** Transfer::Connection_LibCurl ****************************
 ********************************************************************************
 */
Connection::Ptr Connection_LibCurl::New (const Options& options)
{
    return Connection::Ptr{make_shared<Rep_> (options)};
}
#endif
