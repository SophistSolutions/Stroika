/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaType_h_
#define _Stroika_Foundation_DataExchange_InternetMediaType_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Containers/Mapping.h"
#include "../DataExchange/Atom.h"
#include "../Execution/VirtualConstant.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Need some registration mechanism so IsTextFormat() results can be extended.
 *              For example, with HealthFrame, we need to return YES for IsTextFormat() for certiain application/x-... formats.
 *      @todo   Perhaps use optional for mapping as performance hack (but makes object larger so not clearly good idea)
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;

    /**
     *  MIME content-types are also sometimes referred to as 'Internet media type'.
     *
     *  \note - this class stores the type, subtype, and parameters, but it does NOT store any comments from the content-type
     */
    class InternetMediaType {
    public:
        using AtomType = DataExchange::Atom<DataExchange::AtomManager_CaseInsensitive>;

    public:
        /**
         *  Note that according to https://tools.ietf.org/html/rfc2045#page-10, these types
         *  are compared in a case insensitive manner (by type/subtype) and the case constructed with
         *  in the string may not be preserved.
         *      "Matching of media type and subtype; is ALWAYS case-insensitive"
         *
         *  If type provided, subType must be as well (require). And no paramters allowed if type is empty.
         *  The one-argument String overload parses the Content-Type in the usual way.
         */
        InternetMediaType ()                         = default;
        InternetMediaType (const InternetMediaType&) = default;
        explicit InternetMediaType (const String& ct);
        explicit InternetMediaType (AtomType type, AtomType subType, const Containers::Mapping<String, String>& parameters = {});
        explicit InternetMediaType (const String& type, const String& subType, const Containers::Mapping<String, String>& parameters = {});

    public:
        /**
         *  Supported RETURN_TYPES:
         *      o   String
         *      o   AtomType
         */
        template <typename RETURN_TYPE = String>
        nonvirtual RETURN_TYPE GetType () const;

    public:
        /**
         *  Supported RETURN_TYPES:
         *      o   String
         *      o   AtomType
         */
        template <typename RETURN_TYPE = String>
        nonvirtual RETURN_TYPE GetSubType () const;

    public:
        /**
         */
        nonvirtual Containers::Mapping<String, String> GetParameters () const;

    public:
        /**
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         */
        nonvirtual bool empty () const;

    public:
        /**
         */
        nonvirtual void clear ();

    public:
        /**
         *  \brief returns true if you can expect to treat as some sort of text and reasonably view - like text/html, application/x-ccr, application/x-url, etc...
         *
         *  See also PredefinedInternetMediaType::kText
         */
        nonvirtual bool IsTextFormat () const;

    public:
        /**
         * subtype of image / *     {avoid comment-character}
         */
        nonvirtual bool IsImageFormat () const;

    public:
        /**
         *  Like 'Equals' but only comparing type/subtype (or for AtomType overload only matching type)
         */
        nonvirtual bool Match (const AtomType& type) const;
        nonvirtual bool Match (const InternetMediaType& rhs) const;

    public:
        /**
         *  \brief This function compares similar types, like 
         *         application/healthframe-PHR-Format and
         *         application/healthframe-PHR-Format-2 etc
         *         and returns true iff the given type is a prefix (case insensitive)
         *         of the argument more general one. The types must match, and the
         *         parameters are ignored.
         *
         *  \note Change as of Stroika v2.1d27 - now only checks prefix of
         *        subtype - type must match - and now ignores parameters.
         */
        nonvirtual bool IsA (const InternetMediaType& moreGeneralType) const;

    public:
        struct ThreeWayComparer;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        AtomType                            fType_;
        AtomType                            fSubType_;
        Containers::Mapping<String, String> fParameters_{String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive}};
    };
    template <>
    nonvirtual String InternetMediaType::As () const;
    template <>
    nonvirtual wstring InternetMediaType::As () const;

    /**
     *  @see Common::ThreeWayComparer<> template
     *
     *  \note NO GUARANTEE about the meaning of the ordering? for now. May use atom ordering
     *        or case insensitive string ordering, or other. Just legal total ordering.
     *
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    struct InternetMediaType::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        nonvirtual int operator() (const InternetMediaType& lhs, const InternetMediaType& rhs) const;
    };

    /**
     *  Basic operator overloads with the obvious meaning, and simply indirect to @InternetMediaType::ThreeWayComparer ()
     *
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    bool operator< (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator<= (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator== (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator!= (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator>= (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator> (const InternetMediaType& lhs, const InternetMediaType& rhs);

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
        namespace PRIVATE_ {

            const InternetMediaType::AtomType& Text_Type ();
            const InternetMediaType::AtomType& Image_Type ();
            const InternetMediaType::AtomType& Application_Type ();

            const InternetMediaType& OctetStream_CT ();
            const InternetMediaType& Image_PNG_CT ();
            const InternetMediaType& Image_GIF_CT ();
            const InternetMediaType& Image_JPEG_CT ();
            const InternetMediaType& Text_HTML_CT ();
            const InternetMediaType& Text_XHTML_CT ();
            const InternetMediaType& Text_XML_CT ();
            const InternetMediaType& Text_PLAIN_CT ();
            const InternetMediaType& Text_CSV_CT ();
            const InternetMediaType& JSON_CT ();
            const InternetMediaType& PDF_CT ();
            const InternetMediaType& URL_CT ();
            const InternetMediaType& XML_CT ();
            const InternetMediaType& XSLT_CT ();
            const InternetMediaType& JavaArchive_CT ();
            const InternetMediaType& Application_RTF_CT ();
            const InternetMediaType& Application_Zip_CT ();
        }
    }

    namespace InternetMediaTypes::Types {

        /**
         *  \brief 'text'
         *
         *  This is the major type (atom) making up a class of InternetMediaTypes.
         */
        constexpr Execution::VirtualConstant<InternetMediaType::AtomType, PredefinedInternetMediaType::PRIVATE_::Text_Type> kText;

        /**
         *  \brief 'image'
         *
         *  This is the major type (atom) making up a class of InternetMediaTypes.
         */
        constexpr Execution::VirtualConstant<InternetMediaType::AtomType, PredefinedInternetMediaType::PRIVATE_::Image_Type> kImage;

        /**
         *  \brief 'image'
         *
         *  This is the major type (atom) making up a class of InternetMediaTypes.
         */
        constexpr Execution::VirtualConstant<InternetMediaType::AtomType, PredefinedInternetMediaType::PRIVATE_::Application_Type> kApplication;

    }

    namespace InternetMediaTypes {

        /**
         *  application/octet-stream
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::OctetStream_CT> kOctetStream;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Image_PNG_CT> kImage_PNG;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Image_GIF_CT> kImage_GIF;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Image_JPEG_CT> kImage_JPEG;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Text_HTML_CT> kText_HTML;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Text_XHTML_CT> kText_XHTML;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Text_XML_CT> kText_XML;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Text_PLAIN_CT> kText_PLAIN;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Text_CSV_CT> kText_CSV;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::JSON_CT> kJSON;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::PDF_CT> kPDF;

        /**
         * very unclear what to use, no clear standard!
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::URL_CT> kURL;

        /**
         *  application/x-xslt
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::XSLT_CT> kApplication_XSLT;

        /**
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::JavaArchive_CT> kJavaArchive;

        /**
         * Microsoft RTF - Rich Text Format
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Application_RTF_CT> kApplication_RTF;

        /**
         *  application/zip
         */
        constexpr Execution::VirtualConstant<InternetMediaType, PredefinedInternetMediaType::PRIVATE_::Application_Zip_CT> kApplication_Zip;

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetMediaType.inl"

#endif /*_Stroika_Foundation_DataExchange_InternetMediaType_h_*/
