/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Cache/SynchronizedLRUCache.h"

#include "Cache.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;
using namespace Stroika::Foundation::Time;

using Stroika::Foundation::Cache::SynchronizedLRUCache;

namespace {

    struct DefaultCacheRep_ : Transfer::Cache::Rep {

        using Element = Transfer::Cache::Element;

        /**
         */
        virtual optional<Response> OnBeforeFetch (Request* request) override
        {
            // @todo -- see if its in cache, and if so, maybe add conditional if-modfied-since or -if-etag
            // and under somecircumstances, just return the correct answer if its cachable without any call
            // @todo TRICKY - must somehow lockdown (or stick into response) a copy of the response to generate IF
            // we get a NOT-MODIFIED, because the data COULD be gone from the cache by the time we get the not-modified.
            // ? OR - we need to be able to re - request ) a less desirable workaround (and maybe harder todo as well)
            //
            // Maybe have 'CONTEXT' object which also must be passed in as a parameter here so we can store the context
            // in our parents stackframe (so gets cleaned up at right time)
            return nullopt;
        }

        virtual void OnAfterFetch (Request* request, Response* response) override
        {
            RequireNotNull (request);
            RequireNotNull (response);
            switch (response->GetStatus ()) {
                case HTTP::StatusCodes::kOK: {
                    // @todo add result to cache
                    // but must look at headers closely first
                } break;
                case HTTP::StatusCodes::kNotModified: {
                    // @todo replaces response value with right answer on 304
                    // lookup cache value and return it - updating any needed http headers stored in cache
                } break;
                default: {
                    // ignored
                } break;
            }
        }

        /**
         * if not cleared, external cache can be re-used
         */
        virtual void ClearCache () override
        {
        }

        virtual optional<Element> Lookup (const URI& url) const override
        {
            return fCache_.Lookup (url);
        }

#if 0
     *  \par Example Usage
     *      \code
     *          LRUCache<string, string> tmp (3);
     *          tmp.Add ("a", "1");
     *          tmp.Add ("b", "2");
     *          tmp.Add ("c", "3");
     *          tmp.Add ("d", "4");
     *          VerifyTestResult (not tmp.Lookup ("a").has_value ());
     *          VerifyTestResult (tmp.Lookup ("b") == "2");
     *          VerifyTestResult (tmp.Lookup ("d") == "4");
     *      \endcode
#endif
        SynchronizedLRUCache<URI, Element> fCache_{101};
        // we want to use hash stuff but then need hash<URI>
        ///pair<string, string>{}, 3, 10, hash<string>{}
    };

}
/*
 ********************************************************************************
 **************************** Transfer::Cache ***********************************
 ********************************************************************************
 */
Transfer::Cache::Ptr Transfer::Cache::CreateDefault ()
{
    return Ptr{make_shared<DefaultCacheRep_> ()};
}
