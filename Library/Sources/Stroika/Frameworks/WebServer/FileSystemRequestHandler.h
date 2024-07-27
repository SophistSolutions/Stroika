/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_FileSystemRequestHandler_h_
#define _Stroika_Framework_WebServer_FileSystemRequestHandler_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "Stroika/Foundation/IO/Network/HTTP/CacheControl.h"

#include "Stroika/Frameworks/WebServer/Router.h"

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
    };

    struct FileSystemRequestHandler::Options {
        /**
         *  \brief typically starts with '/' since HTTP GET argument must be root relative
         *
         *  but if you have a fURLPrefix2Strip = "/Files/" that means 
         *      GET /Files/phred
         *
         *  will look 'program EXE relative' for the file named 'phred' (convert /Files/phred, then strip leading "/Files/"
         * 
         *  Defaults to "/"
         */
        optional<String> fURLPrefix2Strip;

        /**
         *  @todo DOCS
         */
        optional<Sequence<String>> fDefaultIndexFileNames;

        /**
         *  NYI - see http://stroika-bugs.sophists.com/browse/STK-732
         */
        enum ETagStrategy {
            eDont,
            eDateStamp,
            eDigest,
        };

        /**
         * ReportETags
         *      @todo NYI
         *  NYI - see http://stroika-bugs.sophists.com/browse/STK-732
         */
        optional<ETagStrategy> fETagStrategy;

        /**
         */
        static constexpr ETagStrategy kDefault_ETagStrategy{eDigest};

        // @todo
        // ETagCacheSize (ether digest or date stamp - whether we keep in RAM idea of current value
        // to respond without read (note date check is still a form of read); but this cache is less
        // useful IF using datestamp etag strategy
        // @see http://stroika-bugs.sophists.com/browse/STK-732

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
         *              pair<RegularExpression, CacheControl>{RegularExpression{".*[0-9a-fA-F]+\\.(js|css|js\\.map)", CompareOptions::eCaseInsensitive}, CacheControl::kImmutable},
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
