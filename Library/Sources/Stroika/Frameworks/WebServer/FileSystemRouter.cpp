/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "../../Foundation/IO/FileSystem/Common.h"
#include "../../Foundation/IO/FileSystem/FileInputStream.h"
#include "../../Foundation/IO/FileSystem/PathName.h"
#include "../../Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "../../Foundation/Streams/InputStream.h"

#include "FileSystemRouter.h"

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
        filesystem::path fFSRoot_;
        optional<String> fURLPrefix2Strip;
        Sequence<String> fDefaultIndexFileNames;

        FSRouterRep_ (const String& filesystemRoot, const optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames)
            : fFSRoot_ (filesystem::canonical (filesystem::path (filesystemRoot.As<wstring> ())))
            , fURLPrefix2Strip (urlPrefix2Strip)
            , fDefaultIndexFileNames (defaultIndexFileNames)
        {
        }
        void HandleMessage (Message* m)
        {
            static const DataExchange::InternetMediaTypeRegistry kMediaTypesRegistry_ = DataExchange::InternetMediaTypeRegistry::Default ();
            // super primitive draft
            RequireNotNull (m);
            String fn{ExtractFileName_ (m)};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}...FileSystemRouter...HandleMessage", L"relURL='%s', serving fn='%s'", m->PeekRequest ()->GetURL ().GetAuthorityRelativeResource ().c_str (), fn.c_str ())};
#endif
            try {
                Response&              response = *m->PeekResponse ();
                InputStream<byte>::Ptr in{FileInputStream::New (fn)};
                response.write (in.ReadAll ());
                if (optional<InternetMediaType> oMediaType = kMediaTypesRegistry_.GetAssociatedContentType (fn)) {
                    response.SetContentType (*oMediaType);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"content-type: %s", oMediaType->ToString ().c_str ());
#endif
                }
            }
            catch (const system_error& e) {
#if qCompilerAndStdLib_error_code_compare_condition_Buggy
                // not sure how to workaround this, but fixed in latest gcc's
#endif
                if (e.code () == errc::no_such_file_or_directory) {
                    Execution::Throw (ClientErrorException{StatusCodes::kNotFound});
                }
                else {
                    Execution::ReThrow ();
                }
            }
        }
        String ExtractFileName_ (const Message* m) const
        {
            const Request& request        = *m->PeekRequest ();
            String         urlHostRelPath = request.GetURL ().GetAbsPath<String> ().SubString (1);
            if (fURLPrefix2Strip) {
                if (urlHostRelPath.StartsWith (*fURLPrefix2Strip)) {
                    urlHostRelPath = urlHostRelPath.SubString (fURLPrefix2Strip->length ());
                    if (urlHostRelPath.StartsWith (L"/")) {
                        urlHostRelPath = urlHostRelPath.SubString (1);
                    }
                }
                else {
                    Execution::Throw (ClientErrorException{StatusCodes::kNotFound});
                }
            }
            if ((urlHostRelPath.empty () or urlHostRelPath.EndsWith ('/')) and not fDefaultIndexFileNames.empty ()) {
                //@todo tmphack - need to try a bunch and look for 'access'
                urlHostRelPath += fDefaultIndexFileNames[0];
            }
            return (fFSRoot_ / filesystem::path (urlHostRelPath.As<wstring> ())).wstring ();
        }
    };

    thread_local shared_ptr<FSRouterRep_> tBuilding_;
}

/*
 ********************************************************************************
 ************************* WebServer::FileSystemRouter **************************
 ********************************************************************************
 */
FileSystemRouter::FileSystemRouter (const String& filesystemRoot, const optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames)
    : RequestHandler ((tBuilding_ = make_shared<FSRouterRep_> (filesystemRoot, urlPrefix2Strip, defaultIndexFileNames), [rep = tBuilding_] (Message* m) -> void { rep->HandleMessage (m); }))
{
    Assert (tBuilding_ != nullptr); // @todo use this hack to add instance variable to FileSystemRouter so it can be referenced from above handler
    tBuilding_ = nullptr;
}
