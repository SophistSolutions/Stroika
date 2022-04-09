/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "../../Foundation/Debug/TimingTrace.h"
#include "../../Foundation/IO/FileSystem/Common.h"
#include "../../Foundation/IO/FileSystem/FileInputStream.h"
#include "../../Foundation/IO/FileSystem/PathName.h"
#include "../../Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "../../Foundation/Streams/InputStream.h"

#include "FileSystemRequestHandler.h"

using std::byte;

using namespace Stroika::Foundation;
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
        optional<String>                              fURLPrefix2Strip;
        Sequence<String>                              fDefaultIndexFileNames;
        vector<pair<RegularExpression, CacheControl>> fCacheControlSettings;

        FSRouterRep_ (
            const filesystem::path&                                          filesystemRoot,
            const optional<String>&                                          urlPrefix2Strip,
            const Sequence<String>&                                          defaultIndexFileNames,
            const optional<Sequence<pair<RegularExpression, CacheControl>>>& cacheControlSettings)
            : fFSRoot_{filesystem::canonical (filesystemRoot)}
            , fURLPrefix2Strip{urlPrefix2Strip}
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
#if qDefaultTracingOn
            Debug::TimingTrace ttrc{L"FSRouterRep_::HandleMessage", .001}; // prelim - gather info on whether worth supporting ETAGs etc - why is this sometimes somewhat slow
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
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}...FileSystemRequestHandler...HandleMessage", L"relURL=%s, serving fn=%s", Characters::ToString (m->request ().url ().GetAuthorityRelativeResource ()).c_str (), Characters::ToString (fn).c_str ())};
#endif
            try {
                Response&              response = m->rwResponse ();
                InputStream<byte>::Ptr in{FileInputStream::New (fn)};
                if (optional<InternetMediaType> oMediaType = InternetMediaTypeRegistry::Get ().GetAssociatedContentType (fn.extension ())) {
                    response.contentType = *oMediaType;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"content-type: %s", oMediaType->ToString ().c_str ());
#endif
                }
                ApplyCacheControl_ (response, urlHostRelPath);
                response.write (in.ReadAll ());
            }
            catch (const system_error& e) {
#if qCompilerAndStdLib_error_code_compare_condition_Buggy
                // not sure how to workaround this, but fixed in the latest gcc (GLIBCXX 9)
#endif
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
                if (urlRelPath.Match (i.first)) {
                    r.rwHeaders ().cacheControl = i.second;
                    break; // just apply first
                }
            }
        }
        String ExtractURLHostRelPath_ (const Message* m) const
        {
            const Request& request        = m->request ();
            String         urlHostRelPath = request.url ().GetAbsPath<String> ().SubString (1);
            if (fURLPrefix2Strip) {
                if (urlHostRelPath.StartsWith (*fURLPrefix2Strip)) {
                    urlHostRelPath = urlHostRelPath.SubString (fURLPrefix2Strip->length ());
                    if (urlHostRelPath.StartsWith (L"/"sv)) {
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
    : RequestHandler{[rep = make_shared<FSRouterRep_> (filesystemRoot, options.fURLPrefix2Strip, Memory::NullCoalesce (options.fDefaultIndexFileNames), options.fCacheControlSettings)] (Message* m) -> void { rep->HandleMessage (m); }}
{
}
