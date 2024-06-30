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
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using IO::Network::HTTP::ClientErrorException;
using PropertyChangedEventResultType = Common::PropertyCommon::PropertyChangedEventResultType;

using Debug::AssertExternallySynchronizedMutex;
using Memory::BLOB;

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
    /**
     *  Very confused about what is going on here. Only tested with HTTP 1.1, but both chrome and msft edge dont
     *  handle transfer-encoding the way I would have expected.
     * 
     *  This works FINE with curl - decoding the deflate just fine.
     */
    constexpr bool kTransferEncodingCompressionDoesntAppearToWorkWithBrowsers_ = true;
}

namespace {
    constexpr char kCRLF_[] = "\r\n";
}

Response::Response (Response&& src)
    // Would be nice to use inherited src move, but PITA, because then would need to duplicate creating the properties below.
    : Response{src.fSocket_, src.fProtocolOutputStream_, src.headers ()}
{
    fState_                                     = src.fState_;
    fHeadMode_                                  = src.fHeadMode_;
    fAborted_                                   = src.fAborted_;
    fAutoTransferChunkSize_                     = src.fAutoTransferChunkSize_;
    fBodyEncoding_                              = move (src.fBodyEncoding_);
    fBodyRawStream_                             = move (src.fBodyRawStream_);
    fBodyRawStreamLength_                       = move (src.fBodyRawStreamLength_);
    fBodyRowStreamLengthWhenLastChunkGenerated_ = move (src.fBodyRowStreamLengthWhenLastChunkGenerated_);
    fBodyCompressedStream_                      = move (src.fBodyCompressedStream_);
    fUseOutStream_                              = move (src.fUseOutStream_);
    fCodePage_                                  = move (src.fCodePage_);
    fCodeCvt_                                   = move (src.fCodeCvt_);
    fETagDigester_                              = move (src.fETagDigester_);
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
                          Require (thisObj->fBodyRawStreamLength_ == 0);
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
    , codeCvt{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::codeCvt);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        if (thisObj->fCodeCvt_ == nullopt) {
            AssertExternallySynchronizedMutex::WriteContext declareContext2{const_cast<Response*> (thisObj)->_fThisAssertExternallySynchronized};
            // https://stroika.atlassian.net/browse/STK-983
            thisObj->fCodeCvt_ = Characters::CodeCvt<>{thisObj->fCodePage_};
        }
        return *thisObj->fCodeCvt_;
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
                   Require (thisObj->fBodyRawStreamLength_ == 0);
                   bool diff           = thisObj->fCodePage_ != newCodePage;
                   thisObj->fCodePage_ = newCodePage;
                   if (diff) {
                       if (auto ct = thisObj->headers ().contentType ()) {
                           thisObj->rwHeaders ().contentType = thisObj->AdjustContentTypeForCodePageIfNeeded_ (*ct);
                       }
                       thisObj->fCodeCvt_ = nullopt;
                   }
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
    , hasEntityBody{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::hasEntityBody);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        if (thisObj->fHeadMode_) {
            return false;
        }
        if (thisObj->status () == HTTP::StatusCodes::kNotModified) {
            return false;
        }
        return thisObj->fBodyRawStreamLength_ > 0; //??? - better way to tell???
    }}
    , headersCanBeSet{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::headersCanBeSet);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fState_ == State::ePreparingHeaders;
    }}
    , headMode{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
                   const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::headMode);
                   AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                   return thisObj->fHeadMode_;
               },
               [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& newHeadMode) {
                   Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::headMode);
                   AssertExternallySynchronizedMutex::WriteContext declareContext{thisObj->_fThisAssertExternallySynchronized};
                   Require (thisObj->fState_ == State::ePreparingHeaders);
                   if (newHeadMode) {
                       thisObj->fHeadMode_ = true;
                   }
                   else {
                       Require (thisObj->fHeadMode_ == newHeadMode);
                   }
               }}
    , responseAborted{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::responseAborted);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fAborted_;
    }}
    , responseCompleted{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::responseCompleted);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fState_ == State::eCompleted;
    }}
    , responseStatusSent{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::responseStatusSent);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fState_ != State::ePreparingHeaders;
    }}
    , state{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Response* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Response::state);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        return thisObj->fState_;
    }}
    , fSocket_{s}
    , fProtocolOutputStream_{outStream}
    , fUseOutStream_{Streams::BufferedOutputStream::New<byte> (outStream)}
    , fBodyRawStream_{Streams::SharedMemoryStream::New<byte> (Streams::SharedMemoryStream::Options{
          .fInternallySynchronized = Execution::InternallySynchronized::eNotKnownInternallySynchronized, .fSeekable = false})}
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
        if (this->fHeadMode_) {
            // https://stackoverflow.com/questions/27868314/avoiding-content-length-in-head-response
            //      // https://www.rfc-editor.org/rfc/rfc7231#section-4.3.2
            //      The server SHOULD send the same header fields in response to a HEAD request as it would
            //      have sent if the request had been a GET,
            //      except that the payload header fields (Section 3.3) MAY be omitted.
            return nullopt;
        }
        // in non-chunked mode, we count on all the 'writes' having been completed
        if (fBodyCompressedStream_ != nullptr) {
            return fBodyCompressedStream_.GetOffset ();
        }
        if (this->fBodyRawStream_ != nullptr) {
            return fBodyRawStreamLength_;
        }
        AssertNotReached ();
        return nullopt;
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
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::StateTransition_", "from={}, to={}"_f, static_cast<State> (fState_), static_cast<State> (to)};
#endif
    Require (fState_ <= to);
    if (to != fState_) {
        if (fState_ == State::ePreparingHeaders and to == State::eHeadersSent) {
            ApplyBodyEncodingIfNeeded_ ();
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

void Response::ApplyBodyEncodingIfNeeded_ ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::ApplyBodyEncodingIfNeeded_", "this->bodyEncoding={}"_f, fBodyEncoding_};
#endif
    if (fState_ == State::ePreparingHeaders and fBodyEncoding_ and fBodyCompressedStream_ == nullptr) {
        auto applyBodyEncoding = [this] (HTTP::ContentEncoding ce) {
            if (this->chunkedTransferMode ()) {
                if (kTransferEncodingCompressionDoesntAppearToWorkWithBrowsers_) {
                    rwHeaders ().contentEncoding = ce;
                }
                else {
                    HTTP::TransferEncodings htc = *headers ().transferEncoding ();
                    htc.Prepend (HTTP::TransferEncoding{ce.As<HTTP::TransferEncoding::AtomType> ()});
                    rwHeaders ().transferEncoding = htc;
                }
            }
            else {
                rwHeaders ().contentEncoding = ce;
            }
        };
        Compression::Ptr currentCompression;
        if (fBodyEncoding_->Contains (HTTP::ContentEncoding::kDeflate)) {
            constexpr auto compressOpts = Compression::Deflate::Compress::Options{.fCompressionLevel = 1.0f}; // @todo config option - passed in - didn't seem to help here
            currentCompression = Compression::Deflate::Compress::New (compressOpts);
            applyBodyEncoding (HTTP::ContentEncoding::kDeflate);
        }
        if (currentCompression) {
            // compressed stream reads from the raw body stream
            fBodyCompressedStream_ = currentCompression.Transform (fBodyRawStream_);
        }
    }
}

void Response::WriteChunk_ (span<const byte> rawBytes)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::WriteChunk_", "rawBytes=byte[{}]{}"_f, rawBytes.size (), rawBytes};
#endif
    // note rawBytes maybe empty - in fact the final chunk is always empty
    string n = CString::Format ("%x\r\n", static_cast<unsigned int> (rawBytes.size ()));
    fUseOutStream_.WriteRaw (span{n.data (), n.size ()});
    fUseOutStream_.Write (rawBytes);
    fUseOutStream_.WriteRaw (span{kCRLF_, strlen (kCRLF_)});
}

InternetMediaType Response::AdjustContentTypeForCodePageIfNeeded_ (const InternetMediaType& ct) const
{
    if (InternetMediaTypeRegistry::Get ().IsTextFormat (ct)) {
        using AtomType = InternetMediaType::AtomType;
        // Don't override already specifed characterset
        Containers::Mapping<String, String> params = ct.GetParameters ();
        params.Add ("charset"sv, Characters::GetCharsetString (fCodePage_), AddReplaceMode::eAddIfMissing);
        return InternetMediaType{ct.GetType<AtomType> (), ct.GetSubType<AtomType> (), ct.GetSuffix (), params};
    }
    return ct;
}

void Response::Flush ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::Flush", "fState_ = {}"_f, static_cast<State> (fState_)};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};

    if (fState_ == State::ePreparingHeaders) {
        StateTransition_ (State::eHeadersSent); // this flushes the headers
    }
    Assert (fState_ >= State::eHeadersSent);
    fUseOutStream_.Flush ();
}

bool Response::End ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::End()"};
    DbgTrace ("*this={}"_f, ToString ());
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    if (fState_ != State::eCompleted) {
        try {
            fBodyRawStream_.CloseWrite (); // if any avail to read, must write that chunk...

            ApplyBodyEncodingIfNeeded_ (); // normally done in flush, but that would be too late

            // If NOT in chunked mode, Accumulate the body we will write before our initial flush, so that we have the right contentLength header
            // Note this is needed even for HEAD Method, since we may need to emit the right Content-Length header even if there is
            // no body
            optional<BLOB> body2Write;
            if (not chunkedTransferMode ()) {
                if (fBodyCompressedStream_ != nullptr) {
                    body2Write = fBodyCompressedStream_.ReadAll ();
                }
                else {
                    body2Write = fBodyRawStream_.ReadAll ();
                }
            }
            if (fState_ == State::ePreparingHeaders) {
                StateTransition_ (State::eHeadersSent); // this flushes the headers
            }
            Assert (fState_ >= State::eHeadersSent);

            if (chunkedTransferMode () and not fHeadMode_) {
                if (fBodyCompressedStream_ != nullptr) {
                    auto b = fBodyCompressedStream_.ReadAll ();
                    if (not b.empty ()) {
                        WriteChunk_ (b);
                    }
                }
                else {
                    auto b = fBodyRawStream_.ReadAll ();
                    if (not b.empty ()) {
                        WriteChunk_ (b);
                    }
                }
                WriteChunk_ (span<const byte>{}); // an empty chunk marks the end of transfer encoding
            }

            if (fState_ == State::ePreparingHeaders) {
                StateTransition_ (State::eHeadersSent); // this flushes the headers
            }
            if (body2Write) {
                Assert (not chunkedTransferMode ());
                Assert (fState_ != State::eCompleted); // We PREVENT any writes when completed
                // See https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html - body must not be sent for not-modified, HEAD, etc
                if (this->hasEntityBody ()) {
                    fUseOutStream_.Write (*body2Write);
                }
            }
            Assert (fState_ >= State::eHeadersSent);
            StateTransition_ (State::eCompleted);
            fUseOutStream_.Flush ();
        }
        catch (...) {
            DbgTrace ("Exception during Response::End () automaticaly triggers Response::Abort()"_f);
            Abort ();
            Ensure (this->responseAborted ());
            Ensure (this->responseCompleted ());
            Execution::ReThrow (); // but still rethrow so caller can see failure
        }
    }
    Ensure (fState_ == State::eCompleted);
    return not fAborted_;
}

void Response::Abort ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::Abort()"};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    if (fState_ != State::eCompleted) {
        fState_   = State::eCompleted;
        fAborted_ = true;
        fUseOutStream_.Abort ();
        fSocket_.Close ();
        if (fBodyRawStream_ != nullptr) {
            fBodyRawStream_.Close ();
        }
    }
    Ensure (fState_ == State::eCompleted);
}

void Response::Redirect (const URI& url)
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    Require (this->headersCanBeSet ());
    if (fBodyRawStream_ != nullptr) {
        fBodyRawStream_.Close ();
    }

    // PERHAPS should clear some header values???
    auto& updatableHeaders      = this->rwHeaders ();
    updatableHeaders.connection = IO::Network::HTTP::Headers::eClose;
    updatableHeaders.location   = url;
    this->status                = HTTP::StatusCodes::kMovedPermanently;
    if (fState_ == State::ePreparingHeaders) {
        StateTransition_ (State::eHeadersSent); // this flushes the headers
    }
    Assert (fState_ >= State::eHeadersSent);
    StateTransition_ (State::eCompleted);
}

void Response::write (const span<const byte>& bytes)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Response::write()", "bytes=byte[{}]{}"_f, bytes.size (), bytes};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    Require (not this->responseCompleted ());
    Require (not this->responseStatusSent () or chunkedTransferMode ());
    if (fETagDigester_) {
        fETagDigester_->Write (bytes); // @todo - FIX - To send ETAG - we must use a trailer!!! - maybe other things as well!
    }
    fBodyRawStream_.Write (bytes);
    fBodyRawStreamLength_ += bytes.size ();
    Assert (fBodyRawStreamLength_ == fBodyRawStream_.GetWriteOffset ());

    size_t thisChunkSize = fBodyRawStreamLength_ - fBodyRowStreamLengthWhenLastChunkGenerated_; // compute when to chunk based on raw write size not compressed size cuz easier - for now --LGP 2024-06-26
    if (thisChunkSize > fAutoTransferChunkSize_.value_or (kAutomaticTransferChunkSize_Default) and not chunkedTransferMode ()) {
        rwHeaders ().transferEncoding = HTTP::TransferEncoding::kChunked;
    }

    if (fState_ == State::ePreparingHeaders and chunkedTransferMode ()) {
        // must send first draft of headers before we can begin chunked transfer emitting
        // but cannot flush if not chunked transfer mode, cuz then we would be freezing content-length (unless we were handed it
        // externally, but we don't currently have an API for that -- LGP 2024-06-25 - could add promisedContentLength property(but that would disallow our compression)
        // maybe if/when we support trailers we wont need to worry about this.
        StateTransition_ (State::eHeadersSent); // this sends the headers (not necessarily all the way out over wire but through our pipeline)
        Assert (fState_ >= State::eHeadersSent);
    }

    FlushNextChunkIfNeeded_ ();
}

template <>
void Response::write (const String& s)
{
    Memory::StackBuffer<Character> maybeIgnoreBuf1;
    span<const Character>          thisData = s.GetData (&maybeIgnoreBuf1);
    string                         cpStr    = codeCvt ().String2Bytes<string> (thisData);
    if (not cpStr.empty ()) {
        write (as_bytes (span{cpStr.c_str (), cpStr.length ()}));
    }
}

void Response::printf (const wchar_t* format, ...)
{
    ////DEPRECATED
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    va_list                                         argsList;
    va_start (argsList, format);
    String tmp = Characters::FormatV (format, argsList);
    va_end (argsList);
    write (tmp);
}

void Response::FlushNextChunkIfNeeded_ ()
{
    if (not fHeadMode_ and chunkedTransferMode ()) {
        size_t thisChunkSize = fBodyRawStreamLength_ - fBodyRowStreamLengthWhenLastChunkGenerated_; // compute when to chunk based on raw write size not compressed size cuz easier - for now --LGP 2024-06-26
        if (thisChunkSize > fAutoTransferChunkSize_.value_or (kAutomaticTransferChunkSize_Default)) {
            BLOB data2Write;
            if (this->fBodyCompressedStream_ != nullptr) {
                if (auto o = fBodyCompressedStream_.ReadAllAvailable ()) {
                    data2Write = *o;
                }
            }
            else {
                optional<Memory::InlineBuffer<byte>> aa = fBodyRawStream_.ReadAllAvailable ();
                if (aa.has_value ()) {
                    data2Write = BLOB{*aa};
                }
            }
            // Don't write empty chunks as they would mark the end of the response
            if (not data2Write.empty ()) {
                WriteChunk_ (data2Write);
                fBodyRowStreamLengthWhenLastChunkGenerated_ = fBodyRawStreamLength_;
            }
        }
    }
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
    //sb << "fBodyRawStream_: "sv << fBodyRawStream_ << ", "sv;       // @todo write seek pos, non-null etc
    //sb << "ProtocolOutputStream: "sv << fProtocolOutputStream_ << ", "sv;       // @todo write seek pos, non-null etc
    //sb << "fBodyCompressedStream_: "sv << fBodyCompressedStream_ << ", "sv;       // @todo write seek pos, non-null etc
    sb << "HeadMode: "sv << fHeadMode_ << ", "sv;
    sb << "ETagDigester: "sv << fETagDigester_.has_value ();
    sb << "}"sv;
    return sb;
}
