/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaType_h_
#define _Stroika_Foundation_DataExchange_InternetMediaType_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Containers/Mapping.h"
#include "../DataExchange/Atom.h"
#include "../DataExchange/DefaultSerializer.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange {

    using Characters::String;

    /**
     *  MIME content-types are also sometimes referred to as 'Internet media type'.
     *
     *  References:
     *      o   https://en.wikipedia.org/wiki/Media_type
     *      o   https://tools.ietf.org/html/rfc2045#section-5.1
     *      o   https://tools.ietf.org/html/rfc2046
     *
     *  From https://en.wikipedia.org/wiki/Media_type:
     *      type "/" [tree "."] subtype ["+" suffix] *[";" parameter]
     *
     *      The currently registered types are: 
     *          application, audio, example, font, image, message, model, multipart, text and video
     *
     *  \note - this class stores the type, subtype, suffix, and parameters, but it does NOT store any comments from the content-type
     *
     *  \note The 'tree' is just merged into the 'subtype'
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
     *
     *          NO GUARANTEE about the meaning of the ordering? for now. May use atom ordering
     *          or case insensitive string ordering, or other. Just legal total ordering.
     *
     *  \see @see InternetMediaTypeRegistry for properties of a given media type
     *
     *  TODO:
     *      @todo consider losing empty/clear members, and if they are ever needed, use optional<InternetMediaType>
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
        explicit InternetMediaType (AtomType type, AtomType subType, optional<AtomType> suffix, const Containers::Mapping<String, String>& parameters = {});
        explicit InternetMediaType (const String& type, const String& subType, const Containers::Mapping<String, String>& parameters = {});
        explicit InternetMediaType (const String& type, const String& subType, const optional<String>& suffix, const Containers::Mapping<String, String>& parameters = {});

    public:
        /**
         *  \brief Gets the primary (major) type of the full internet media type (as a string or atom)
         *
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
         *  \brief this is the +XXX part of the internet media type (e.g. +xml) and is often omitted (but note this omits the + sign)
         *
         *  Supported RETURN_TYPES:
         *      o   String
         *      o   AtomType
         */
        template <typename RETURN_TYPE = String>
        nonvirtual optional<RETURN_TYPE> GetSuffix () const;

    public:
        /**
         */
        nonvirtual Containers::Mapping<String, String> GetParameters () const;

    public:
        /**
         *  \brief convert to type T
         *      supported types:
         *          o   String
         *          o   wstring
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         *      @todo consider losing empty/clear members, and if they are ever needed, use optional<InternetMediaType>
         */
        nonvirtual bool empty () const;

    public:
        /**
         *      @todo consider losing empty/clear members, and if they are ever needed, use optional<InternetMediaType>
         */
        nonvirtual void clear ();

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const InternetMediaType& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const InternetMediaType& rhs) const;
#endif

    private:
        nonvirtual Common::strong_ordering THREEWAYCOMPARE_ (const InternetMediaType& rhs) const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        AtomType                            fType_;
        AtomType                            fSubType_;
        optional<AtomType>                  fSuffix_;
        Containers::Mapping<String, String> fParameters_{String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive}};

#if __cpp_impl_three_way_comparison < 201907
    private:
        friend bool operator< (const InternetMediaType& lhs, const InternetMediaType& rhs);
        friend bool operator<= (const InternetMediaType& lhs, const InternetMediaType& rhs);
        friend bool operator== (const InternetMediaType& lhs, const InternetMediaType& rhs);
        friend bool operator!= (const InternetMediaType& lhs, const InternetMediaType& rhs);
        friend bool operator>= (const InternetMediaType& lhs, const InternetMediaType& rhs);
        friend bool operator> (const InternetMediaType& lhs, const InternetMediaType& rhs);
#endif
    };
    template <>
    nonvirtual String InternetMediaType::As () const;
    template <>
    nonvirtual wstring InternetMediaType::As () const;

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic operator overloads with the obvious meaning, and simply indirect to @InternetMediaType::ThreeWayComparer ()
     */
    bool operator< (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator<= (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator== (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator!= (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator>= (const InternetMediaType& lhs, const InternetMediaType& rhs);
    bool operator> (const InternetMediaType& lhs, const InternetMediaType& rhs);
#endif

}

namespace std {
    template <>
    struct hash<Stroika::Foundation::DataExchange::InternetMediaType> {
        size_t operator() (const Stroika::Foundation::DataExchange::InternetMediaType& arg) const;
    };
}
namespace Stroika::Foundation::DataExchange {
    template <>
    struct DefaultSerializer<Stroika::Foundation::DataExchange::InternetMediaType> {
        Memory::BLOB operator() (const Stroika::Foundation::DataExchange::InternetMediaType& arg) const;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetMediaType.inl"

#endif /*_Stroika_Foundation_DataExchange_InternetMediaType_h_*/
