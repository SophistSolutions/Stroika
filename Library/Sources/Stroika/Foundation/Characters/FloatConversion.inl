/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_inl_
#define _Stroika_Foundation_Characters_FloatConversion_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Containers/Common.h"
#include "../Memory/SmallStackBuffer.h"

namespace Stroika {
    namespace Foundation {
        namespace Characters {

            /*
             ********************************************************************************
             *********************** Float2StringOptions::Precision *************************
             ********************************************************************************
             */
            inline Float2StringOptions::Precision::Precision (unsigned int p)
                : fPrecision (p)
            {
            }

            /*
             ********************************************************************************
             ****************************** Float2StringOptions *****************************
             ********************************************************************************
             */
            inline Float2StringOptions::Float2StringOptions (const std::locale& l)
                : fUseLocale_ (l)
            {
            }
            inline Float2StringOptions::Float2StringOptions (std::ios_base::fmtflags fmtFlags)
                : fFmtFlags_ (fmtFlags)
            {
            }
            inline Float2StringOptions::Float2StringOptions (Precision precision)
                : fPrecision_ (precision.fPrecision)
            {
            }
            inline Float2StringOptions::Float2StringOptions (ScientificNotationType scientificNotation)
                : fScientificNotation_ (scientificNotation)
            {
            }
            inline Float2StringOptions::Float2StringOptions (TrimTrailingZerosType trimTrailingZeros)
                : fTrimTrailingZeros_ (trimTrailingZeros == TrimTrailingZerosType::eTrim)
            {
            }
            inline Float2StringOptions::Float2StringOptions (const Float2StringOptions& b1, const Float2StringOptions& b2)
                : Float2StringOptions (b1)
            {
                b2.fPrecision_.CopyToIf (&fPrecision_);
                b2.fFmtFlags_.CopyToIf (&fFmtFlags_);
                b2.fUseLocale_.CopyToIf (&fUseLocale_);
                b2.fTrimTrailingZeros_.CopyToIf (&fTrimTrailingZeros_);
                b2.fScientificNotation_.CopyToIf (&fScientificNotation_);
            }
            template <typename... ARGS>
            inline Float2StringOptions::Float2StringOptions (const Float2StringOptions& b1, const Float2StringOptions& b2, ARGS&&... args)
                : Float2StringOptions (Float2StringOptions{b1, b2}, forward<ARGS> (args)...)
            {
            }
            inline Memory::Optional<unsigned int> Float2StringOptions::GetPrecision () const
            {
                return fPrecision_;
            }
            inline Memory::Optional<bool> Float2StringOptions::GetTrimTrailingZeros () const
            {
                return fTrimTrailingZeros_;
            }
            inline Memory::Optional<std::locale> Float2StringOptions::GetUseLocale () const
            {
                return fUseLocale_;
            }
            inline Memory::Optional<Float2StringOptions::ScientificNotationType> Float2StringOptions::GetScientificNotation () const
            {
                return fScientificNotation_;
            }
            inline Memory::Optional<std::ios_base::fmtflags> Float2StringOptions::GetIOSFmtFlags () const
            {
                return fFmtFlags_;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Characters_FloatConversion_inl_*/
