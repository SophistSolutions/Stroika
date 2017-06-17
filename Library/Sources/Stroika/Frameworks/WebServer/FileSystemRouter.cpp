/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "../../Foundation/IO/FileAccessException.h"
#include "../../Foundation/IO/FileSystem/FileInputStream.h"
#include "../../Foundation/IO/FileSystem/PathName.h"
#include "../../Foundation/IO/Network/HTTP/Exception.h"
#include "../../Foundation/Streams/InputStream.h"

#include "FileSystemRouter.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    struct FSRouterRep_ {
        String           fFSRoot_;
        Optional<String> fURLPrefix2Strip;
        Sequence<String> fDefaultIndexFileNames;

        FSRouterRep_ (const String& filesystemRoot, const Optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames)
            : fFSRoot_ (IO::FileSystem::AssureDirectoryPathSlashTerminated (filesystemRoot))
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
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}...FileSystemRouter...HandleMessage", L"relURL='%s', fn='%s'", m->PeekRequest ()->GetURL ().GetHostRelativePath ().c_str (), fn.c_str ())};
#endif
            try {
                InputStream<Byte>::Ptr in{FileInputStream::mk (fn)};
                m->PeekResponse ()->write (in.ReadAll ());
                if (Optional<InternetMediaType> oMediaType = kMediaTypesRegistry_.GetAssociatedContentType (fn)) {
                    m->PeekResponse ()->SetContentType (*oMediaType);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"content-type: %s", oMediaType->ToString ().c_str ())
#endif
                }
            }
            catch (const IO::FileAccessException&) {
                Execution::Throw (IO::Network::HTTP::Exception{StatusCodes::kNotFound});
            }
        }
        String ExtractFileName_ (const Message* m) const
        {
            String urlRelPath = m->PeekRequest ()->GetURL ().GetHostRelativePath ();
            if (fURLPrefix2Strip) {
                if (urlRelPath.StartsWith (*fURLPrefix2Strip)) {
                    urlRelPath = urlRelPath.SubString (fURLPrefix2Strip->length ());
                }
                else {
                    Execution::Throw (IO::Network::HTTP::Exception{StatusCodes::kNotFound});
                }
            }
            if ((urlRelPath.empty () or urlRelPath.EndsWith ('/')) and not fDefaultIndexFileNames.empty ()) {
                //@todo tmphack - need to try a bunch and look for 'access'
                urlRelPath += fDefaultIndexFileNames[0];
            }
            return fFSRoot_ + urlRelPath;
        }
    };

    thread_local shared_ptr<FSRouterRep_> tBuilding_;
}

/*
 ********************************************************************************
 ************************* WebServer::FileSystemRouter **************************
 ********************************************************************************
 */
FileSystemRouter::FileSystemRouter (const String& filesystemRoot, const Optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames)
    : RequestHandler ((tBuilding_ = make_shared<FSRouterRep_> (filesystemRoot, urlPrefix2Strip, defaultIndexFileNames), [rep = tBuilding_](Message * m)->void { rep->HandleMessage (m); }))
{
    Assert (tBuilding_ != nullptr); // @todo use this hack to add instance variable to FileSystemRouter so it can be referenced from above handler
    tBuilding_ = nullptr;
}
