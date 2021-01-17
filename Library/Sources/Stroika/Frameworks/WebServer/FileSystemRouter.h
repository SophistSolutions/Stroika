/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_FileSystemRouter_h_
#define _Stroika_Framework_WebServer_FileSystemRouter_h_ 1

#include "../StroikaPreComp.h"

#include <filesystem>
#include <optional>

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

    class Router;

    /**
     *  @todo unclear if this should BE a Router or RequestHandler, but I htink Handler, and so rename!
     */
    class FileSystemRouter : public RequestHandler {
    public:
        struct Options;
        static const Options kDefaultOptions;

    public:
        /**
         *  Any route to apply the handler, must match ALL argument constraints.
         */
        FileSystemRouter (const filesystem::path& filesystemRoot, const optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames = {});
    };

    struct FileSystemRouter::Options {
        /**
         */
        optional<String> fURLPrefix2Strip;
        optional<Sequence<String>> fDefaultIndexFileNames;


        /// todo add options

        enum ETagStrategy {
            eDont,
            eDateStamp,
            eDigest,
        };
        // ReportETags
        optional < ETagStrategy> fETagStrategy;


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
        //

    };
    inline const FileSystemRouter::Options FileSystemRouter::kDefaultOptions;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileSystemRouter.inl"

#endif /*_Stroika_Framework_WebServer_FileSystemRouter_h_*/
