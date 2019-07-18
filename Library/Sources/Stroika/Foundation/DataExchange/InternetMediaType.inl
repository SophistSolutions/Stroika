/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_InternetMediaType_inl_
#define _Stroika_Foundation_DataExchange_InternetMediaType_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/ModuleInit.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ******************************** InternetMediaType *****************************
     ********************************************************************************
     */
    inline InternetMediaType::InternetMediaType (AtomType type, AtomType subType, const Containers::Mapping<String, String>& parameters)
        : fType_ (type)
        , fSubType_ (subType)
        , fParameters_ (parameters)
    {
        Require (type.empty () == subType.empty ());
        Require (not type.empty () or parameters.empty ());
    }
    inline InternetMediaType::InternetMediaType (const String& type, const String& subType, const Containers::Mapping<String, String>& parameters)
        : fType_ (type)
        , fSubType_ (subType)
        , fParameters_ (parameters)
    {
        Require (type.empty () == subType.empty ());
        Require (not type.empty () or parameters.empty ());
    }
    inline bool InternetMediaType::empty () const
    {
        Assert (fType_.empty () == fSubType_.empty ());
        Assert (not fType_.empty () or fParameters_.empty ());
        return fType_.empty ();
    }
    inline void InternetMediaType::clear ()
    {
        fType_.clear ();
        fSubType_.clear ();
        fParameters_.clear ();
    }
    template <>
    inline auto InternetMediaType::GetType () const -> AtomType
    {
        return fType_;
    }
    template <>
    inline auto InternetMediaType::GetType () const -> String
    {
        return fType_.GetPrintName ();
    }
    template <>
    inline auto InternetMediaType::GetSubType () const -> AtomType
    {
        return fSubType_;
    }
    template <>
    inline auto InternetMediaType::GetSubType () const -> String
    {
        return fSubType_.GetPrintName ();
    }
    inline Containers::Mapping<String, String> InternetMediaType::GetParameters () const
    {
        return fParameters_;
    }
    inline bool InternetMediaType::Match (const InternetMediaType& rhs) const
    {
        if (GetType<AtomType> () != rhs.GetType<AtomType> ()) {
            return false;
        }
        if (GetSubType<AtomType> () != rhs.GetSubType<AtomType> ()) {
            return false;
        }
        return true;
    }
    inline bool InternetMediaType::Match (const AtomType& rhsType) const
    {
        return (GetType<AtomType> () == rhsType);
    }
    inline bool InternetMediaType::IsA (const InternetMediaType& moreGeneralType) const
    {
        if (GetType<AtomType> () != moreGeneralType.GetType<AtomType> ()) {
            return false;
        }
        // compare just the subtypes, for prefix equals
        return GetSubType<String> ().StartsWith (moreGeneralType.GetSubType<String> (), Characters::CompareOptions::eCaseInsensitive);
    }
    inline bool InternetMediaType::Equals (const InternetMediaType& rhs) const
    {
        return fType_ == rhs.fType_ and fSubType_ == rhs.fSubType_ and fParameters_ == rhs.fParameters_;
    }
    inline int InternetMediaType::Compare (const InternetMediaType& rhs) const
    {
        return InternetMediaType::ThreeWayComparer{}(*this, rhs);
    }
    template <>
    inline wstring InternetMediaType::As () const
    {
        return As<String> ().As<wstring> ();
    }

    /*
     ********************************************************************************
     ********************************** TypeMatchesAny ******************************
     ********************************************************************************
     */
    template <typename CONTAINER>
    bool TypeMatchesAny (const CONTAINER& types, const InternetMediaType& type2SeeIfMatchesAny)
    {
        for (auto i = types.begin (); i != types.end (); ++i) {
            if (i->IsA (type2SeeIfMatchesAny)) {
                return true;
            }
        }
        return false;
    }

    /*
     ********************************************************************************
     *********************** InternetMediaType operators ****************************
     ********************************************************************************
     */
    inline bool operator< (const InternetMediaType& lhs, const InternetMediaType& rhs)
    {
        return InternetMediaType::ThreeWayComparer{}(lhs, rhs) < 0;
    }
    inline bool operator<= (const InternetMediaType& lhs, const InternetMediaType& rhs)
    {
        return InternetMediaType::ThreeWayComparer{}(lhs, rhs) <= 0;
    }
    inline bool operator== (const InternetMediaType& lhs, const InternetMediaType& rhs)
    {
        return InternetMediaType::ThreeWayComparer{}(lhs, rhs) == 0;
    }
    inline bool operator!= (const InternetMediaType& lhs, const InternetMediaType& rhs)
    {
        return InternetMediaType::ThreeWayComparer{}(lhs, rhs) != 0;
    }
    inline bool operator>= (const InternetMediaType& lhs, const InternetMediaType& rhs)
    {
        return InternetMediaType::ThreeWayComparer{}(lhs, rhs) >= 0;
    }
    inline bool operator> (const InternetMediaType& lhs, const InternetMediaType& rhs)
    {
        return InternetMediaType::ThreeWayComparer{}(lhs, rhs) > 0;
    }

}

namespace Stroika::Foundation::DataExchange::Private_ {
    struct InternetMediaType_ModuleData_ {
        InternetMediaType_ModuleData_ ();
        ~InternetMediaType_ModuleData_ () = default;

        const InternetMediaType::AtomType kText_Type;
        const InternetMediaType::AtomType kImage_Type;
        const InternetMediaType::AtomType kApplication_Type;

        const InternetMediaType kOctetStream_CT;

        const InternetMediaType kImage_PNG_CT;
        const InternetMediaType kImage_GIF_CT;
        const InternetMediaType kImage_JPEG_CT;

        const InternetMediaType kText_HTML_CT;
        const InternetMediaType kText_XHTML_CT;
        const InternetMediaType kText_XML_CT;
        const InternetMediaType kText_PLAIN_CT;
        const InternetMediaType kText_CSV_CT;

        const InternetMediaType kJSON_CT;

        const InternetMediaType kPDF_CT;
        const InternetMediaType kURL_CT;

        const InternetMediaType kXML_CT;

        const InternetMediaType kXSLT_CT;
        const InternetMediaType kJavaArchive_CT;
        const InternetMediaType kApplication_RTF_CT;
        const InternetMediaType kApplication_Zip_CT;
    };
}

namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::DataExchange::Private_::InternetMediaType_ModuleData_> _Stroika_Foundation_DataExchange_InternetMediaType_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}

namespace Stroika::Foundation::DataExchange::PredefinedInternetMediaType::PRIVATE_ {

    inline const InternetMediaType::AtomType& Text_Type () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_Type; }
    inline const InternetMediaType::AtomType& Image_Type () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_Type; }
    inline const InternetMediaType::AtomType& Application_Type () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kApplication_Type; }

    inline const InternetMediaType& OctetStream_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kOctetStream_CT; }

    inline const InternetMediaType& Image_PNG_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_PNG_CT; }
    inline const InternetMediaType& Image_GIF_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_GIF_CT; }
    inline const InternetMediaType& Image_JPEG_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kImage_JPEG_CT; }

    inline const InternetMediaType& Text_HTML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_HTML_CT; }
    inline const InternetMediaType& Text_XHTML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_XHTML_CT; }
    inline const InternetMediaType& Text_XML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_XML_CT; }
    inline const InternetMediaType& Text_PLAIN_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_PLAIN_CT; }
    inline const InternetMediaType& Text_CSV_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kText_CSV_CT; }

    inline const InternetMediaType& JSON_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kJSON_CT; }

    inline const InternetMediaType& PDF_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kPDF_CT; }

    inline const InternetMediaType& URL_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kURL_CT; }

    inline const InternetMediaType& XML_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kXML_CT; }

    inline const InternetMediaType& XSLT_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kXSLT_CT; }

    inline const InternetMediaType& JavaArchive_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kJavaArchive_CT; }

    inline const InternetMediaType& Application_RTF_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kApplication_RTF_CT; }

    inline const InternetMediaType& Application_Zip_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kApplication_Zip_CT; }
}

#endif /*_Stroika_Foundation_DataExchange_InternetMediaType_inl_*/
