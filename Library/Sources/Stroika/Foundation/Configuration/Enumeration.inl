/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Enumeration_inl_
#define _Stroika_Foundation_Configuration_Enumeration_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Configuration {

    /*
     ********************************************************************************
     ******************************** Configuration::Inc ****************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr ENUM Inc (ENUM e)
    {
        return ToEnum<ENUM> (ToInt (e) + 1);
    }

    /*
     ********************************************************************************
     ****************************** Configuration::ToInt ****************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr typename underlying_type<ENUM>::type ToInt (ENUM e)
    {
        // https://stroika.atlassian.net/browse/STK-549
        //static_assert (ENUM::eSTART <= e and e <= ENUM::eEND);
        return static_cast<typename underlying_type<ENUM>::type> (e);
    }

    /*
     ********************************************************************************
     ********************* Configuration::GetDistanceSpanned ************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr make_unsigned_t<typename underlying_type<ENUM>::type> GetDistanceSpanned ()
    {
        return static_cast<make_unsigned_t<typename underlying_type<ENUM>::type>> (ENUM::eCOUNT);
    }

    /*
     ********************************************************************************
     ***************************** Configuration::ToEnum ****************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr ENUM ToEnum (typename underlying_type<ENUM>::type e)
    {
        // https://stroika.atlassian.net/browse/STK-549
        //static_assert (ENUM::eSTART <= static_cast<ENUM> (e) and static_cast<ENUM> (e) <= ENUM::eEND);
        return static_cast<ENUM> (e);
    }

    /*
     ********************************************************************************
     ******************** Configuration::OffsetFromStart ****************************
     ********************************************************************************
     */
    template <typename ENUM>
    inline constexpr make_unsigned_t<typename underlying_type<ENUM>::type> OffsetFromStart (ENUM e)
    {
        // https://stroika.atlassian.net/browse/STK-549
        //static_assert (ENUM::eSTART <= e and e <= ENUM::eEND);
        return static_cast<make_unsigned_t<typename underlying_type<ENUM>::type>> (ToInt (e) - ToInt (ENUM::eSTART));
    }
    template <typename ENUM>
    inline constexpr ENUM OffsetFromStart (make_unsigned_t<typename underlying_type<ENUM>::type> offset)
    {
        return ToEnum<ENUM> (offset + ENUM::eSTART);
    }

    /*
     ********************************************************************************
     ************************** Configuration::EnumNames ****************************
     ********************************************************************************
     */
    template <typename ENUM_TYPE>
    inline EnumNames<ENUM_TYPE>::EnumNames (const initializer_list<EnumName<ENUM_TYPE>>& origEnumNames)
        : fEnumNames_ ()
    {
        // @todo find some way to INITIALZIE the static array.... - needed for constexpr function!
        // @see qCANNOT_FIGURE_OUT_HOW_TO_INIT_STD_ARRAY_FROM_STD_INITIALIZER_
        auto oi = fEnumNames_.begin ();
        for (EnumName<ENUM_TYPE> i : origEnumNames) {
            Require (oi != fEnumNames_.end ());
            *oi = i;
            ++oi;
        }
        RequireItemsOrderedByEnumValue_ ();
    }
    template <typename ENUM_TYPE>
    inline constexpr EnumNames<ENUM_TYPE>::EnumNames (const typename EnumNames<ENUM_TYPE>::BasicArrayInitializer& init)
        : fEnumNames_ (init)
    {
        RequireItemsOrderedByEnumValue_ ();
    }
    template <typename ENUM_TYPE>
    template <size_t N>
    inline constexpr EnumNames<ENUM_TYPE>::EnumNames (const EnumName<ENUM_TYPE> origEnumNames[N])
        : fEnumNames_ (origEnumNames)
    {
        RequireItemsOrderedByEnumValue_ ();
    }
    template <typename ENUM_TYPE>
    inline EnumNames<ENUM_TYPE>::operator initializer_list<EnumName<ENUM_TYPE>> () const
    {
        return fEnumNames_;
    }
    template <typename ENUM_TYPE>
    inline typename EnumNames<ENUM_TYPE>::const_iterator EnumNames<ENUM_TYPE>::begin () const
    {
        return fEnumNames_.begin ();
    }
    template <typename ENUM_TYPE>
    inline typename EnumNames<ENUM_TYPE>::const_iterator EnumNames<ENUM_TYPE>::end () const
    {
        return fEnumNames_.end ();
    }
    template <typename ENUM_TYPE>
    inline constexpr size_t EnumNames<ENUM_TYPE>::size () const
    {
        return fEnumNames_.size ();
    }
    template <typename ENUM_TYPE>
    inline constexpr const wchar_t* EnumNames<ENUM_TYPE>::PeekName (ENUM_TYPE e) const
    {
        if (e == ENUM_TYPE::eEND) {
            return nullptr;
        }
        if constexpr (qDebug) {
            Require (OffsetFromStart<ENUM_TYPE> (e) < fEnumNames_.size ());
            auto refImpl = [this](ENUM_TYPE e) -> const wchar_t* {
                for (auto i : fEnumNames_) {
                    if (i.first == e) {
                        return i.second;
                    }
                }
                return nullptr;
            };
            Ensure (refImpl (e) == fEnumNames_[OffsetFromStart<ENUM_TYPE> (e)].second);
        }
        return fEnumNames_[OffsetFromStart<ENUM_TYPE> (e)].second;
    }
    template <typename ENUM_TYPE>
    inline const wchar_t* EnumNames<ENUM_TYPE>::GetName (ENUM_TYPE e) const
    {
        auto tmp = PeekName (e);
        RequireNotNull (tmp);
        return tmp;
    }
    template <typename ENUM_TYPE>
    const ENUM_TYPE* EnumNames<ENUM_TYPE>::PeekValue (const wchar_t* name) const
    {
        /*
            *  NB: this is only safe returning an internal pointer, because the pointer is internal to
            *  static, immudatable data - the basic_array associated with this EnumNames<> structure.
            */
        RequireNotNull (name);
        for (const_iterator i = fEnumNames_.begin (); i != fEnumNames_.end (); ++i) {
            if (::wcscmp (i->second, name) == 0) {
                return &i->first;
            }
        }
        return nullptr;
    }
    template <typename ENUM_TYPE>
    inline ENUM_TYPE EnumNames<ENUM_TYPE>::GetValue (const wchar_t* name) const
    {
        const ENUM_TYPE* tmp = PeekValue (name);
        RequireNotNull (tmp);
        return *tmp;
    }
    template <typename ENUM_TYPE>
    template <typename NOT_FOUND_EXCEPTION>
    inline ENUM_TYPE EnumNames<ENUM_TYPE>::GetValue (const wchar_t* name, const NOT_FOUND_EXCEPTION& notFoundException) const
    {
        RequireNotNull (name);
        const ENUM_TYPE* tmp = PeekValue (name);
        if (tmp == nullptr)
            [[UNLIKELY_ATTR]]
            {
                //Execution::Throw (notFoundException);
                throw (notFoundException);
            }
        return *tmp;
    }
    template <typename ENUM_TYPE>
    inline constexpr void EnumNames<ENUM_TYPE>::RequireItemsOrderedByEnumValue_ () const
    {
        Require (static_cast<size_t> (ENUM_TYPE::eCOUNT) == fEnumNames_.size ());
        using IndexType = make_unsigned_t<typename underlying_type<ENUM_TYPE>::type>;
        for (IndexType i = 0; i < static_cast<IndexType> (ENUM_TYPE::eCOUNT); ++i) {
            Require (OffsetFromStart<ENUM_TYPE> (fEnumNames_[i].first) == i);
        }
    }

    /*
     ********************************************************************************
     ************************** Configuration::DefaultNames *************************
     ********************************************************************************
     */
    template <typename ENUM_TYPE>
    inline DefaultNames<ENUM_TYPE>::DefaultNames ()
        : EnumNames<ENUM_TYPE> (k)
    {
    }

}
#endif /*_Stroika_Foundation_Configuration_Enumeration_inl_*/
