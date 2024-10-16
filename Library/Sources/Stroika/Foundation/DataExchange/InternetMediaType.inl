/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ******************************** InternetMediaType *****************************
     ********************************************************************************
     */
    inline InternetMediaType::InternetMediaType (AtomType type, AtomType subType, optional<AtomType> suffix,
                                                 const Containers::Mapping<String, String>& parameters)
        : fType_{type}
        , fSubType_{subType}
        , fSuffix_{suffix}
        , fParameters_{String::EqualsComparer{Characters::eCaseInsensitive}, parameters}
    {
        Require (type.empty () == subType.empty ());
        Require (not type.empty () or parameters.empty ()); // dont specify params without type
        Require (not type.empty () or suffix == nullopt);   // dont specify suffix without type
    }
    inline InternetMediaType::InternetMediaType (AtomType type, AtomType subType, const Containers::Mapping<String, String>& parameters)
        : InternetMediaType{type, subType, nullopt, parameters}
    {
    }
    inline InternetMediaType::InternetMediaType (const String& type, const String& subType, const Containers::Mapping<String, String>& parameters)
        : InternetMediaType{static_cast<AtomType> (type), static_cast<AtomType> (subType), parameters}
    {
    }
    inline InternetMediaType::InternetMediaType (const String& type, const String& subType, const optional<String>& suffix,
                                                 const Containers::Mapping<String, String>& parameters)
        : InternetMediaType{static_cast<AtomType> (type), static_cast<AtomType> (subType),
                            suffix == nullopt ? nullopt : optional<AtomType>{static_cast<AtomType> (*suffix)}, parameters}
    {
    }
    inline bool InternetMediaType::empty () const
    {
        Assert (fType_.empty () == fSubType_.empty ());
        Assert (not fType_.empty () or fParameters_.empty ()); // dont specify params without type
        Assert (not fType_.empty () or fSuffix_ == nullopt);   // dont specify suffix without type
        return fType_.empty ();
    }
    inline void InternetMediaType::clear ()
    {
        fType_.clear ();
        fSubType_.clear ();
        fSuffix_ = nullopt;
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
    template <>
    inline auto InternetMediaType::GetSuffix () const -> optional<AtomType>
    {
        return fSuffix_;
    }
    template <>
    inline auto InternetMediaType::GetSuffix () const -> optional<String>
    {
        return fSuffix_ ? optional<String>{fSuffix_->GetPrintName ()} : nullopt;
    }
    inline Containers::Mapping<String, String> InternetMediaType::GetParameters () const
    {
        return fParameters_;
    }
    template <>
    inline wstring InternetMediaType::As () const
    {
        return As<String> ().As<wstring> ();
    }
    inline strong_ordering InternetMediaType::operator<=> (const InternetMediaType& rhs) const
    {
        return THREEWAYCOMPARE_ (rhs);
    }
    inline bool InternetMediaType::operator== (const InternetMediaType& rhs) const
    {
        return THREEWAYCOMPARE_ (rhs) == 0;
    }

}
