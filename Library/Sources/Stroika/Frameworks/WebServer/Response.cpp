/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/Compression/Deflate.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"

#include "Response.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using IO::Network::HTTP::ClientErrorException;
using PropertyChangedEventResultType = Common::PropertyCommon::PropertyChangedEventResultType;

using Debug::AssertExternallySynchronizedMutex;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 **************************** WebServer::Response *******************************
 ********************************************************************************
 */
namespace {
    // Based on looking at a handful of typical file sizes...5k to 80k, but ave around
    // 25 and median a bit above 32k. Small, not very representative sampling. And the more we use
    // subscripts (script src=x) this number could shrink.
    //
    // MAY want to switch to using InlineBuffer<byte> - but before doing so, do some cleanups of its bugs and make sure
    // its optimized about how much it copies etc. Its really only tuned for POD-types (OK here but not necessarily good about reallocs).
    //
    //      -- LGP 2011-07-06
    constexpr size_t kResponseBufferReallocChunkSizeReserve_ = 16 * 1024;
}

namespace {
    constexpr char kCRLF_[] = "\r\n";
}

Response::Response (Response&& src)
    // Would be nice to use inherited src move, but PITA, because then would need to duplicate creating the properties below.
    : Response{src.fSocket_, src.fUnderlyingOutStream_, src.headers ()}
{
    fState_        = src.fState_;
    fUseOutStream_ = move (src.fUseOutStream_);
    fCodePage_     = src.fCodePage_;
    fBodyBytes_    = move (src.fBodyBytes_);
    fHeadMode_     = src.fHeadMode_;
    fETagDigester_ = move (src.fETagDigester_);
}

Response::Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream::Ptr<byte>& outStream, const optional<HTTP::Headers>& initialHeaders)
    : inherited{initialHeaders}
    , autoComputeETag{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                          const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::autoComputeETag);
                          AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                          return thisObj->fETagDigester_.has_value ();
                      },
                      [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const bool newAutoComputeETag) {
                          Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::autoComputeETag);
                          AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                          Require (thisObj->state () == State::ePreparingHeaders);
                          Assert (thisObj->fBodyBytes_.empty ());
                          if (newAutoComputeETag) {
                              thisObj->fETagDigester_ = ETagDigester_{};
                          }
                          else {
                              thisObj->fETagDigester_ = nullopt;
                          }
                      }}
    , automaticTransferChunkSize{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
              const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::automaticTransferChunkSize);
              AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
              return thisObj->fAutoTransferChunkSize_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const optional<size_t>& newAutoComputeValue) {
              Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::automaticTransferChunkSize);
              AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
              Require (thisObj->state () == State::ePreparingHeaders);
              thisObj->fAutoTransferChunkSize_ = newAutoComputeValue;
          }}
    , codePage{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                   const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::codePage);
                   AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                   return thisObj->fCodePage_;
               },
               [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newCodePage) {
                   Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::codePage);
                   AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                   Require (thisObj->headersCanBeSet ());
                   Require (thisObj->fBodyBytes_.empty ());
                   bool diff           = thisObj->fCodePage_ != newCodePage;
                   thisObj->fCodePage_ = newCodePage;
                   if (diff) {
                       if (auto ct = thisObj->headers ().contentType ()) {
                           thisObj->rwHeaders ().contentType = thisObj->AdjustContentTypeForCodePageIfNeeded_ (*ct);
                       }
                   }
               }}
    , bodyEncoding{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                       const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::bodyEncoding);
                       AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                       return thisObj->fBodyEncoding_;
                   },
                   [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newCT) {
                       Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::bodyEncoding);
                       AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                       thisObj->fBodyEncoding_ = newCT;
                   }}
    , chunkedTransferMode{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::chunkedTransferMode);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        auto                                           te = thisObj->headers ().transferEncoding ();
        return te and te->Contains (HTTP::TransferEncoding::kChunked);
    }}
    , contentType{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                      const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::contentType);
                      AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                      return thisObj->headers ().contentType ();
                  },
                  [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newCT) {
                      Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::contentType);
                      AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                      thisObj->rwHeaders ().contentType = newCT;
                  }}
    , state{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::state);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fState_;
    }}
    , headersCanBeSet{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::headersCanBeSet);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fState_ == State::ePreparingHeaders;
    }}
    , responseStatusSent{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::responseStatusSent);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fState_ != State::ePreparingHeaders /* and thisObj->fState_ != State::ePreparingBodyBeforeHeadersSent*/;
    }}
    , responseCompleted{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::responseCompleted);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fState_ == State::eCompleted;
    }}
    , responseAborted{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::responseAborted);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fAborted_;
    }}
    , hasEntityBody{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::hasEntityBody);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        if (thisObj->fHeadMode_) {
            return false;
        }
        if (thisObj->status () == HTTP::StatusCodes::kNotModified) {
            return false;
        }
        return thisObj->fRawBytesWritten_ > 0; //??? - better way to tell???
    }}
    , fSocket_{s}
    , fUnderlyingOutStream_{outStream}
    , fUseOutStream_{Streams::BufferedOutputStream::New<byte> (outStream)}
{
    if constexpr (qDebug) {
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wunused-lambda-capture\""); // sadly no way to [[maybe_unused]] on captures
        this->status.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) {
            Require (not this->responseStatusSent ());
            return PropertyChangedEventResultType::eContinueProcessing;
        });
        this->statusAndOverrideReason.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) {
            Require (not this->responseStatusSent ());
            return PropertyChangedEventResultType::eContinueProcessing;
        });
        this->rwHeaders.rwPropertyReadHandlers ().push_front ([this] (HTTP::Headers* h) {
            Require (this->headersCanBeSet ());
            return h;
        });
        this->rwHeaders.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) {
            Require (this->headersCanBeSet ());
            return PropertyChangedEventResultType::eContinueProcessing;
        });
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wunused-lambda-capture\"");
    }
    this->rwHeaders ().transferEncoding.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) {
        AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
        Require (this->headersCanBeSet ());
        return PropertyChangedEventResultType::eContinueProcessing;
    });
    // auto-compute the content-length unless chunked transfer
    this->rwHeaders ().contentLength.rwPropertyReadHandlers ().push_front ([this] ([[maybe_unused]] const auto& baseValue) -> optional<uint64_t> {
        if (this->chunkedTransferMode ()) {
            return nullopt;
        }
        //if (this->responseStatusSent ()) {
        return GetPossiblyEncodedBody_ ().size ();
        //}
        //return nullopt;
    });
    this->rwHeaders ().contentLength.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) {
        RequireNotReached (); // since v3.0d7 - disallow
                              //        return PropertyChangedEventResultType::eContinueProcessing;
        return PropertyChangedEventResultType::eSilentlyCutOffProcessing;
    });
    // auto-compute the etag if autoComputeETag (fETagDigester_.has_value()) is true
    this->rwHeaders ().ETag.rwPropertyReadHandlers ().push_front ([this] (const auto& baseETagValue) -> optional<HTTP::ETag> {
        // return the current tag 'so far' (if we are auto-computing)
        if (fETagDigester_) {
            auto copy = *fETagDigester_; // copy cuz this could get called multiple times and Complete only allowed to be called once
            return HTTP::ETag{copy.Complete ()};
        }
        return baseETagValue; // if we are not auto-computing
    });
    this->rwHeaders ().ETag.rwPropertyChangedHandlers ().push_front ([this] ([[maybe_unused]] const auto& propertyChangedEvent) {
        Require (this->headersCanBeSet ());
        AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
        // if someone explicitly sets the etag, then stop auto-computing it
        this->autoComputeETag = false;
        return PropertyChangedEventResultType::eContinueProcessing;
    });
}

void Response::StateTransition_ (State to)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_ || 1
    Debug::TraceContextBumper ctx{"Response::StateTransition_", "from={}, to={}"_f, static_cast<State> (fState_), static_cast<State> (to)};
#endif
    Require (fState_ <= to);
    if (to != fState_) {
        if ((fState_ == State::ePreparingHeaders /* or fState_ == State::ePreparingBodyBeforeHeadersSent*/) and to == State::eHeadersSent) {
            if (fBodyEncoding_) {
                if (fBodyEncoding_->Contains (HTTP::ContentEncoding::kDeflate)) {
                    if (this->chunkedTransferMode ()) {
                        HTTP::TransferEncodings htc = *headers ().transferEncoding ();
                        htc.Append (HTTP::TransferEncoding::kDeflate);
                        rwHeaders ().transferEncoding = htc;
                    }
                    else {
                        rwHeaders ().contentEncoding = HTTP::ContentEncoding::kDeflate;
                        fCurrentCompression_         = DataExchange::Compression::Deflate::Compress::New ();
                    }
                }
            }
            {
                auto   curStatusInfo = this->statusAndOverrideReason ();
                Status curStatus     = get<0> (curStatusInfo);
                String statusMsg =
                    Memory::NullCoalesce (get<1> (curStatusInfo), IO::Network::HTTP::Exception::GetStandardTextForStatus (curStatus, true));
                wstring  version = L"1.1";
                wstring  tmp     = Characters::CString::Format (L"HTTP/%s %d %s\r\n", version.c_str (), curStatus, statusMsg.c_str ());
                u8string utf8    = String{tmp}.AsUTF8 ();
                fUseOutStream_.WriteRaw (span{utf8.data (), utf8.length ()});
            }
            {
                for (const auto& i : this->headers ().As<> ()) {
                    u8string utf8 = Characters::Format ("{}: {}\r\n"_f, i.fKey, i.fValue).AsUTF8 ();
                    fUseOutStream_.WriteRaw (span{utf8.data (), utf8.length ()});
                    DbgTrace ("headers-line-: {}"_f, utf8);
                }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("headers: {}"_f, headers ());
#endif
            }
            fUseOutStream_.WriteRaw (span{kCRLF_, ::strlen (kCRLF_)});
        }

        fState_ = to;
        if constexpr (qDebug) {
            if (to >= State::eHeadersSent and hasEntityBody ()) {
                Assert (chunkedTransferMode () or this->headers ().contentLength ().has_value ()); // I think is is always required, but double check...
            }
        }
    }
}

InternetMediaType Response::AdjustContentTypeForCodePageIfNeeded_ (const InternetMediaType& ct) const
{
    if (DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (ct)) {
        using AtomType = InternetMediaType::AtomType;
        // Don't override already specifed characterset
        Containers::Mapping<String, String> params = ct.GetParameters ();
        params.Add ("charset"sv, Characters::GetCharsetString (fCodePage_), AddReplaceMode::eAddIfMissing);
        return InternetMediaType{ct.GetType<AtomType> (), ct.GetSubType<AtomType> (), ct.GetSuffix (), params};
    }
    return ct;
}

Memory::BLOB Response::GetPossiblyEncodedBody_ () const
{
    if (auto cc = fCurrentCompression_) {
        return cc.Transform (fBodyBytes_);
    }
    return fBodyBytes_;
}

void Response::Flush ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::Flush", "fState_ = {}"_f, static_cast<State> (fState_)};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};

    if (fState_ == State::ePreparingHeaders /* or fState_ == State::ePreparingBodyBeforeHeadersSent*/) {
        StateTransition_ (State::eHeadersSent); // this flushes the headers
    }
    Assert (fState_ >= State::eHeadersSent);

    // write BYTES to fOutStream
    if (not fBodyBytes_.empty ()) {
        Assert (fState_ != State::eCompleted); // We PREVENT any writes when completed
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("bytes.size: {}"_f, static_cast<long long> (fBodyBytes_.size ()));
#endif
        // See https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html - body must not be sent for not-modified
        //              if (not fHeadMode_ and this->status () != HTTP::StatusCodes::kNotModified) {
        if (this->hasEntityBody ()) {
            fUseOutStream_.Write (GetPossiblyEncodedBody_ ());
        }
        fBodyBytes_.clear ();
    }
    if (fState_ != State::eCompleted) {
        fUseOutStream_.Flush ();
    }
    Ensure (fBodyBytes_.empty ());
}

bool Response::End ()
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    if (fState_ != State::eCompleted) {
        try {
            if (chunkedTransferMode ()) {
                constexpr string_view kEndChunk_ = "0\r\n\r\n";
                Assert (as_bytes (span{kEndChunk_}).size () == 5); // not including NUL-byte
                fUseOutStream_.Write (as_bytes (span{kEndChunk_}));
            }
            Flush ();
            StateTransition_ (State::eCompleted);
        }
        catch (...) {
            DbgTrace (L"Exception during Response::End () automaticaly triggers Response::Abort()"_f);
            Abort ();
            Ensure (this->responseAborted ());
            Ensure (this->responseCompleted ());
            Execution::ReThrow (); // but still rethrow so caller can see failure
        }
    }
    Ensure (fState_ == State::eCompleted);
    Ensure (fBodyBytes_.empty ());
    return not fAborted_;
}

void Response::Abort ()
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    if (fState_ != State::eCompleted) {
        fState_   = State::eCompleted;
        fAborted_ = true;
        fUseOutStream_.Abort ();
        fSocket_.Close ();
        fBodyBytes_.clear ();
    }
    Ensure (fState_ == State::eCompleted);
    Ensure (fBodyBytes_.empty ());
}

void Response::Redirect (const URI& url)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    Require (this->headersCanBeSet ());
    fBodyBytes_.clear ();

    // PERHAPS should clear some header values???
    auto& updatableHeaders      = this->rwHeaders ();
    updatableHeaders.connection = IO::Network::HTTP::Headers::eClose;
    updatableHeaders.location   = url;
    this->status                = HTTP::StatusCodes::kMovedPermanently;
    Flush ();
    StateTransition_ (State::eCompleted);
}

void Response::write (const span<const byte>& bytes)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::write()", "bytes=#{}"_f, bytes.size ()};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    Require (not this->responseCompleted ());
    Require (not this->responseStatusSent () or chunkedTransferMode ());
    fRawBytesWritten_ += bytes.size_bytes ();
    if (not bytes.empty ()) {
        if (fETagDigester_) {
            fETagDigester_->Write (bytes);
        }
        if (fState_ == State::ePreparingHeaders and chunkedTransferMode ()) {
            Flush ();
        }
        if (chunkedTransferMode ()) {
            if (not fHeadMode_) {
                Memory::BLOB data2Write = bytes;
                if (auto cc = fCurrentCompression_) {
                    data2Write = cc.Transform (data2Write);
                }
                string n = CString::Format ("%x\r\n", static_cast<unsigned int> (data2Write.size ()));
                fUseOutStream_.WriteRaw (span{n.data (), n.size ()});
                fUseOutStream_.Write (data2Write);
                fUseOutStream_.WriteRaw (span{kCRLF_, strlen (kCRLF_)});
            }
        }
        else {
            // Because for auto-compute - illegal to call flush and then write
            Containers::Support::ReserveTweaks::Reserve4AddN (fBodyBytes_, bytes.size (), kResponseBufferReallocChunkSizeReserve_);
            fBodyBytes_.insert (fBodyBytes_.end (), bytes.begin (), bytes.end ());
        }
    }
}

void Response::write (const wchar_t* s, const wchar_t* e)
{
    Require (s <= e);
    if (s < e) {
        // https://stroika.atlassian.net/browse/STK-983
        string cpStr = Characters::CodeCvt<wchar_t>{fCodePage_}.String2Bytes<string> (wstring{s, e});
        if (not cpStr.empty ()) {
            write (as_bytes (span{cpStr.c_str (), cpStr.length ()}));
        }
    }
}

void Response::printf (const wchar_t* format, ...)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    va_list                                         argsList;
    va_start (argsList, format);
    String tmp = Characters::FormatV (format, argsList);
    va_end (argsList);
    write (tmp);
}

String Response::ToString () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
    StringBuilder                                  sb = inherited::ToString ().SubString (0, -1); // strip trailing '}'
    sb << "Socket: "sv << fSocket_ << ", "sv;
    sb << "chunkedTransferMode: "sv << this->chunkedTransferMode () << ", "sv;
    sb << "hasEntityBody: "sv << this->hasEntityBody () << ", "sv;
    sb << "State: "sv << fState_ << ", "sv;
    sb << "CodePage: "sv << fCodePage_ << ", "sv;
    sb << "BodyBytes: "sv << fBodyBytes_ << ", "sv;
    sb << "HeadMode: "sv << fHeadMode_ << ", "sv;
    sb << "ETagDigester: "sv << fETagDigester_.has_value ();
    sb << "}"sv;
    return sb;
}
