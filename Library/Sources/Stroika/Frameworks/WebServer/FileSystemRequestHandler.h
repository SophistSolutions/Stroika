/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_FileSystemRequestHandler_h_
#define _Stroika_Framework_WebServer_FileSystemRequestHandler_h_ 1

#include "../StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "../../Foundation/IO/Network/HTTP/CacheControl.h"

#include "Router.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Add optional logger feature, to log requests (possibly in standard https://www.w3.org/TR/WD-logfile)
 *
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using Characters::RegularExpression;
    using Characters::String;
    using Containers::Sequence;
    using IO::Network::HTTP::CacheControl;

    class Router;

    /**
     *  @todo unclear if this should BE a Router or RequestHandler, but I htink Handler, and so rename!
     */
    class FileSystemRequestHandler : public RequestHandler {
    public:
        struct Options;
        static const Options kDefaultOptions;

    public:
        /**
         *  Any route to apply the handler, must match ALL argument constraints.
         */
        FileSystemRequestHandler (const filesystem::path& filesystemRoot, const Options& options = kDefaultOptions);
        [[deprecated ("Since Stroika 2.1b10 - use overload with Options object")]] FileSystemRequestHandler (const filesystem::path& filesystemRoot, const optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames = {});
    };

    struct FileSystemRequestHandler::Options {
        /**
         *  @todo DOCS
         */
        optional<String> fURLPrefix2Strip;

        /**
         *  @todo DOCS
         */
        optional<Sequence<String>> fDefaultIndexFileNames;

        /**
         *  NYI - see https://stroika.atlassian.net/browse/STK-732
         */
        enum ETagStrategy {
            eDont,
            eDateStamp,
            eDigest,
        };

        /**
         * ReportETags
         *      @todo NYI
         *  NYI - see https://stroika.atlassian.net/browse/STK-732
         */
        optional<ETagStrategy> fETagStrategy;

        /**
         */
        static constexpr ETagStrategy kDefault_ETagStrategy{eDigest};

        // @todo
        // ETagCacheSize (ether digest or datestamp - whether we keep in RAM idea of current value
        // to respond without read (note date check is still a form of read); but this cache is less
        // useful IF using datestamp etag strategy
        // @see https://stroika.atlassian.net/browse/STK-732

        /**
         *  fCacheControlSettings provides a sequence of RegExp: CacheControl pairs. These are automatically applied
         *  to the host-relative pathname for the resource (foo/bar/blah.gif forward slash even in windows) - and
         *  the first matching regexp - if any - we default to using its associated Cache-Control settings.
         * 
         *  This makes it easy to setup default/automatic rules for applying cache control settings.
         * 
         *  \par Example Usage:
         *      \code
         *          Sequence<pair<RegularExpression, CacheControl>> kFSCacheControlSettings_ {
         *              // webpack generates js/css files with a hex/hash prefix, so those are immutable
         *              pair<RegularExpression, CacheControl>{RegularExpression{L".*[0-9a-fA-F]+\\.(js|css|js\\.map)", CompareOptions::eCaseInsensitive}, CacheControl::kImmutable},
         *              // treat everything else as valid to be cached for a day (very arbitrary)
         *              pair<RegularExpression, CacheControl>{RegularExpression::kAny, CacheControl{.fMaxAge = Duration{24h}.As<int32_t> ()}},
         *          };
         *      \endcode      
         */
        optional<Sequence<pair<RegularExpression, CacheControl>>> fCacheControlSettings;
    };
    inline const FileSystemRequestHandler::Options FileSystemRequestHandler::kDefaultOptions;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileSystemRequestHandler.inl"

#endif /*_Stroika_Framework_WebServer_FileSystemRequestHandler_h_*/
