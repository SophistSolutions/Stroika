/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaType_h_
#define _Stroika_Foundation_DataExchange_InternetMediaType_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"

/**
 * TODO:
 *      @todo   Need some registration mechanism so IsTextFormat() results can be extended.
 *              For example, with HealthFrame, we need to return YES for IsTextFormat() for certiain application/x-... formats.
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {

            using Characters::String;

            /**
             *  MIME content-types are also sometimes referred to as 'Internet media type'
             */
            class InternetMediaType {
            public:
                explicit InternetMediaType (const String& ct = String ());

            public:
                template <typename T>
                nonvirtual T    As () const;
                nonvirtual bool empty () const;
                nonvirtual void clear ();

                // handy helpers -
            public:
                nonvirtual bool IsTextFormat () const;                                // returns true if you can expect to treat as some sort of text and reasonably view - like text/html, application/x-ccr, application/x-url, etc...
                nonvirtual bool IsImageFormat () const;                               // subtype of image/*
                nonvirtual bool IsA (const InternetMediaType& moreGeneralType) const; // synonym for IsSubTypeOfOrEqualTo ()

            public:
                /**
                 */
                nonvirtual bool Equals (const InternetMediaType& rhs) const;

            public:
                /**
                 */
                nonvirtual int Compare (const InternetMediaType& rhs) const;

            public:
                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual String ToString () const;

            private:
                String fType_;
            };
            template <>
            nonvirtual String InternetMediaType::As () const;
            template <>
            nonvirtual wstring InternetMediaType::As () const;

            /**
             */
            bool operator< (const InternetMediaType& lhs, const InternetMediaType& rhs);

            /**
             */
            bool operator<= (const InternetMediaType& lhs, const InternetMediaType& rhs);

            /**
             */
            bool operator== (const InternetMediaType& lhs, const InternetMediaType& rhs);

            /**
             */
            bool operator!= (const InternetMediaType& lhs, const InternetMediaType& rhs);

            /**
             */
            bool operator>= (const InternetMediaType& lhs, const InternetMediaType& rhs);

            /**
             */
            bool operator> (const InternetMediaType& lhs, const InternetMediaType& rhs);

            /**
             */
            bool IsSubTypeOf (const InternetMediaType& moreSpecificType, const InternetMediaType& moreGeneralType);

            /**
             */
            bool IsSubTypeOfOrEqualTo (const InternetMediaType& moreSpecificType, const InternetMediaType& moreGeneralType);

            /**
             */
            template <typename CONTAINER>
            bool TypeMatchesAny (const CONTAINER& types, const InternetMediaType& type2SeeIfMatchesAny);

            /**
             *  NB: these are basically constants, but declaring them as
             *      const   InternetMediaType   kHealthBookURL_CT                   =   ContentType (L"application/x-healthbook-url");
             *  causes the string CTOR for ContentType to be evaluated multiple times - once for each module this CPP file is loaded
             *  into (way overkill - esp if not used).
             *
             *  I tried declaring these as extern const ContentType& kImage_CT;
             *  but that produced problems accessing them at application startup (deadly embrace of startup module issues).
             *
             *  This appears the best compromise. They get initialized once (using the ModuleInit<> code) - and effectively
             *  referenced (after inlining) through an extra pointer, but that should be the limit of the overhead - if the
             *  compilers do a decent job.
             *      -- LGP 2009-05-29
             *
             *  \note
             *      @see http://www.iana.org/assignments/media-types/media-types.xhtml
             *
             *  @todo
             *          https://stroika.atlassian.net/browse/STK-576 - move to InternetMediaTypeRegistry
             */
            namespace PredefinedInternetMediaType {
                const InternetMediaType& OctetStream_CT ();

                const InternetMediaType& Image_CT ();
                const InternetMediaType& Image_PNG_CT ();
                const InternetMediaType& Image_GIF_CT ();
                const InternetMediaType& Image_JPEG_CT ();

                const InternetMediaType& Text_CT ();
                const InternetMediaType& Text_HTML_CT ();
                const InternetMediaType& Text_XHTML_CT ();
                const InternetMediaType& Text_XML_CT ();
                const InternetMediaType& Text_PLAIN_CT ();
                const InternetMediaType& Text_CSV_CT ();

                const InternetMediaType& JSON_CT ();

                const InternetMediaType& PDF_CT ();

                const InternetMediaType& URL_CT (); // very unclear what to use, no clear standard!
                //      -- LGP 2006-07-27

                const InternetMediaType& XML_CT ();

                const InternetMediaType& XSLT_CT ();

                const InternetMediaType& JavaArchive_CT ();

                const InternetMediaType& Application_RTF_CT (); // Microsoft RTF - Rich Text Format

                const InternetMediaType& Application_Zip_CT (); // application/zip
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetMediaType.inl"

#endif /*_Stroika_Foundation_DataExchange_InternetMediaType_h_*/
