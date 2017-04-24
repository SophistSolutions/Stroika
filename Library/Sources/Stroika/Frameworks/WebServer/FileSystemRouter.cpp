/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
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

namespace {
    struct FSRouterRep_ {
        String           fFSRoot_;
        Optional<String> fURLPrefix2Strip;

        FSRouterRep_ (const String& filesystemRoot, const Optional<String>& urlPrefix2Strip)
            : fFSRoot_ (IO::FileSystem::AssureDirectoryPathSlashTerminated (filesystemRoot))
            , fURLPrefix2Strip (urlPrefix2Strip)
        {
        }

        void HandleMessage (Message* m)
        {
            // super primitive draft
            RequireNotNull (m);
            String fn{ExtractFileName_ (m)};
            try {
                InputStream<Byte> in{FileInputStream::mk (fn)};
                m->PeekResponse ()->write (in.ReadAll ());
                m->PeekResponse ()->SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
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
FileSystemRouter::FileSystemRouter (const String& filesystemRoot, const Optional<String>& urlPrefix2Strip)
    : RequestHandler ((tBuilding_ = make_shared<FSRouterRep_> (filesystemRoot, urlPrefix2Strip), [rep = tBuilding_](Message * m)->void { rep->HandleMessage (m); }))
{
    Assert (tBuilding_ != nullptr);
    tBuilding_ = nullptr;
}
