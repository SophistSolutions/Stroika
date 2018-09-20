/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_inl_
#define _Stroika_Foundation_Characters_FloatConversion_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Containers/Common.h"
#include "../Memory/Optional.h"
#include "../Memory/SmallStackBuffer.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     *********************** Float2StringOptions::Precision *************************
     ********************************************************************************
     */
    constexpr Float2StringOptions::Precision::Precision (unsigned int p)
        : fPrecision (p)
    {
    }
    constexpr Float2StringOptions::Precision kDefaultPrecision{6};

    /*
     ********************************************************************************
     ****************************** Float2StringOptions *****************************
     ********************************************************************************
     */
    inline Float2StringOptions::Float2StringOptions (const locale& l)
        : fUseLocale_ (l)
    {
    }
    inline Float2StringOptions::Float2StringOptions (ios_base::fmtflags fmtFlags)
        : fFmtFlags_ (fmtFlags)
    {
    }
    inline Float2StringOptions::Float2StringOptions (Precision precision)
        : fPrecision_ (precision.fPrecision)
    {
    }
    inline Float2StringOptions::Float2StringOptions (FloatFormatType scientificNotation)
        : fFloatFormat_ (scientificNotation)
    {
    }
    inline Float2StringOptions::Float2StringOptions (TrimTrailingZerosType trimTrailingZeros)
        : fTrimTrailingZeros_ (trimTrailingZeros == TrimTrailingZerosType::eTrim)
    {
    }
    inline Float2StringOptions::Float2StringOptions (const Float2StringOptions& b1, const Float2StringOptions& b2)
        : Float2StringOptions (b1)
    {
        Memory::CopyToIf (b2.fPrecision_, &fPrecision_);
        Memory::CopyToIf (b2.fFmtFlags_, &fFmtFlags_);
        Memory::CopyToIf (b2.fUseLocale_, &fUseLocale_);
        Memory::CopyToIf (b2.fTrimTrailingZeros_, &fTrimTrailingZeros_);
        Memory::CopyToIf (b2.fFloatFormat_, &fFloatFormat_);
    }
    template <typename... ARGS>
    inline Float2StringOptions::Float2StringOptions (const Float2StringOptions& b1, const Float2StringOptions& b2, ARGS&&... args)
        : Float2StringOptions (Float2StringOptions{b1, b2}, forward<ARGS> (args)...)
    {
    }
    inline optional<unsigned int> Float2StringOptions::GetPrecision () const
    {
        return fPrecision_;
    }
    inline optional<bool> Float2StringOptions::GetTrimTrailingZeros () const
    {
        return fTrimTrailingZeros_;
    }
    inline optional<locale> Float2StringOptions::GetUseLocale () const
    {
        return fUseLocale_;
    }
    inline optional<Float2StringOptions::FloatFormatType> Float2StringOptions::GetFloatFormat () const
    {
        return fFloatFormat_;
    }
    inline optional<ios_base::fmtflags> Float2StringOptions::GetIOSFmtFlags () const
    {
        return fFmtFlags_;
    }

}

#endif /*_Stroika_Foundation_Characters_FloatConversion_inl_*/
