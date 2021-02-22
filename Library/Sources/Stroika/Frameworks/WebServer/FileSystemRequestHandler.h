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
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
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
         */
        optional<String> fURLPrefix2Strip;

        /**
         */
        optional<Sequence<String>> fDefaultIndexFileNames;

        /**
         *  NYI
         */
        enum ETagStrategy {
            eDont,
            eDateStamp,
            eDigest,
        };
 
        /**
         * ReportETags
         */
        optional<ETagStrategy> fETagStrategy;

        /**
         */
        static constexpr ETagStrategy kDefault_ETagStrategy{eDigest};

        // ETagCacheSize (ether digest or datestamp - whether we keep in RAM idea of current value
        // to respond without read (note date check is still a form of read); but this cache is less
        // useful IF using datestamp etag strategy

        // flag todo auto cache control
        // if true, then ??? maybe have optional
        // CacheControl object based on regexp on filename?
        // that may make more sense cz tmpfile names - like ###.css - we want to mark as lasting forver
        // basically, and we want user to specify what todo for top level HTML file (index.html)
        // EG
        // Mapping<> {// WAG AT REGEX
        //      {RegExp{".*[0-9a-f]+.css"}, CacheControl{CacheControl::ePublic,.fMaxAge=CacheControl::kMaximumAge}},
        //      {RegExp{".*[0-9a-f]+.js"}, CacheControl{CacheControl::ePublic,.fMaxAge=CacheControl::kMaximumAge}},
        //      {RegExp{"index.html"}, CacheControl{CacheControl::ePublic,.fMaxAge=Duration{"PT1D"}}},
        //      
        //  RegExp applied to the relative path name from filesystemRoot, written using '/' style separators
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
