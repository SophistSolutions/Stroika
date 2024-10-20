/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Debug/TimingTrace.h"
#include "Stroika/Foundation/IO/FileSystem/Common.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "FileSystemRequestHandler.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using IO::Network::HTTP::ClientErrorException;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {
    struct FSRouterRep_ {
        filesystem::path                              fFSRoot_;
        String                                        fURLPrefix2Strip_;
        Sequence<String>                              fDefaultIndexFileNames;
        vector<pair<RegularExpression, CacheControl>> fCacheControlSettings;

        FSRouterRep_ (const filesystem::path& filesystemRoot, const optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames,
                      const optional<Sequence<pair<RegularExpression, CacheControl>>>& cacheControlSettings)
            : fFSRoot_{filesystem::canonical (filesystemRoot)}
            , fURLPrefix2Strip_{urlPrefix2Strip.value_or ("/"sv)}
            , fDefaultIndexFileNames{defaultIndexFileNames}
        {
            if (cacheControlSettings) {
                for (const auto& i : *cacheControlSettings) {
                    fCacheControlSettings.push_back (i);
                }
            }
        }
        void HandleMessage (Message* m)
        {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
            Debug::TimingTrace ttrc{"FSRouterRep_::HandleMessage", 1ms}; // prelim - gather info on whether worth supporting ETAGs etc - why is this sometimes somewhat slow
#endif
            /*
             * @todo rewrite to incrementally copy file from stream, not read all into RAM
             */
            using DataExchange::InternetMediaTypeRegistry;
            // super primitive draft
            RequireNotNull (m);
            String           urlHostRelPath{ExtractURLHostRelPath_ (m)};
            filesystem::path fn{fFSRoot_ / filesystem::path{urlHostRelPath.As<wstring> ()}};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{
                Stroika_Foundation_Debug_OptionalizeTraceArgs ("{}...FileSystemRequestHandler...HandleMessage", "relURL={}, serving fn={}"_f,
                                                               m->request ().url ().GetAuthorityRelativeResource (), fn)};
#endif
            try {
                Response&              response = m->rwResponse ();
                InputStream::Ptr<byte> in{FileInputStream::New (fn)};
                if (optional<InternetMediaType> oMediaType = InternetMediaTypeRegistry::sThe->GetAssociatedContentType (fn.extension ())) {
                    response.contentType = *oMediaType;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("content-type: {}"_f, oMediaType->ToString ());
#endif
                }
                ApplyCacheControl_ (response, urlHostRelPath);
                response.write (in.ReadAll ());
            }
            catch (const system_error& e) {
                if (e.code () == errc::no_such_file_or_directory) {
                    Execution::Throw (ClientErrorException{HTTP::StatusCodes::kNotFound});
                }
                else {
                    Execution::ReThrow ();
                }
            }
        }
        void ApplyCacheControl_ (Response& r, const String& urlRelPath) const
        {
            for (const auto& i : fCacheControlSettings) {
                if (urlRelPath.Matches (i.first)) {
                    r.rwHeaders ().cacheControl = i.second;
                    break; // just apply first
                }
            }
        }
        String ExtractURLHostRelPath_ (const Message* m) const
        {
            const Request& request        = m->request ();
            String         urlHostRelPath = request.url ().Normalize (URI::NormalizationStyle::eAggressive).GetAbsPath<String> ();
            Assert (not urlHostRelPath.Contains ("/../")); // so no escape magic - normalize assures
            if (not fURLPrefix2Strip_.empty ()) {
                if (urlHostRelPath.StartsWith (fURLPrefix2Strip_)) {
                    urlHostRelPath = urlHostRelPath.SubString (fURLPrefix2Strip_.length ());
                    if (urlHostRelPath.StartsWith ("/"sv)) {
                        urlHostRelPath = urlHostRelPath.SubString (1);
                    }
                }
                else {
                    Execution::Throw (ClientErrorException{HTTP::StatusCodes::kNotFound});
                }
            }
            if ((urlHostRelPath.empty () or urlHostRelPath.EndsWith ('/')) and not fDefaultIndexFileNames.empty ()) {
                //@todo tmphack - need to try a bunch and look for 'access'
                urlHostRelPath += fDefaultIndexFileNames[0];
            }
            return urlHostRelPath;
        }
    };
}

/*
 ********************************************************************************
 ************************* WebServer::FileSystemRequestHandler ******************
 ********************************************************************************
 */
FileSystemRequestHandler::FileSystemRequestHandler (const filesystem::path& filesystemRoot, const Options& options)
    : RequestHandler{[rep = make_shared<FSRouterRep_> (filesystemRoot, options.fURLPrefix2Strip, Memory::NullCoalesce (options.fDefaultIndexFileNames),
                                                       options.fCacheControlSettings)] (Message* m) -> void { rep->HandleMessage (m); }}
{
}
