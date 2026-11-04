/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Memory/Optional.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ****************************** SignificantFigures ******************************
     ********************************************************************************
     */
    constexpr SignificantFigures::SignificantFigures (unsigned int p)
        : fSignificantFigures_{p}
    {
    }
    constexpr SignificantFigures::SignificantFigures (FullFlag)
        : fSignificantFigures_{} // kFullPrecision
    {
    }
    template <floating_point T>
    constexpr auto SignificantFigures::GetEffectiveSignificantFigures () const -> RepType
    {
        return fSignificantFigures_.value_or (numeric_limits<T>::max_digits10);
    }
    template <IStdBasicStringCompatibleCharacter CHAR>
    constexpr auto SignificantFigures::Calculate (span<const CHAR> number) -> RepType
    {
        // two passes cuz eatLeadingZero depends on scientific, which we find at the end of the text
        bool dotPresent = false;
        bool scientific = false;
        for (const CHAR c : number) {
            switch (c) {
                // Some locales use . (most), but europe uses , for same purpose (and used in ISO-8601)
                case '.':
                case ',':
                    dotPresent = true;
                    break;
                case 'e':
                case 'E':
                    scientific = true;
                    break;
            }
        }
        RepType n{0};
        RepType trailingZeros{0};
        bool    eatLeadingZeros = not scientific;
        bool    gotoDone        = false;
        for (const CHAR c : number) {
            switch (c) {
                case '-':
                case '+':
                    break;
                case '0':
                    if (eatLeadingZeros) {
                        // munch
                    }
                    else {
                        ++n;
                    }
                    ++trailingZeros;
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    eatLeadingZeros = false;
                    trailingZeros   = 0;
                    ++n;
                    break;
                case '.':
                case ',':
                    trailingZeros = 0;
                    break;
                case 'e':
                case 'E':
                    gotoDone = true; // goto Done;   // FIX to use this when c++23
                    break;
            }
            if (gotoDone) {
                break;
            }
        }
        //    Done:
        if (dotPresent) {
            if (eatLeadingZeros) {
                // then the trailing zeros we counted were also leading 0, but wern't added to n so do so now
                n += trailingZeros;
            }
        }
        else {
            n -= trailingZeros;
        }
        return n;
    }

    /**
     *      From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf,
     *      init (basic_streambuf...) initializes precision to 6
     *      Stroika need not maintain that default here, but it seems a sensible one...
     * 
     *  \note defined here instead of header cuz needs to be after SignificantFigures::CTOR.
     */
    constexpr inline SignificantFigures SignificantFigures::kDefault{6};

    /**
     *  \brief Full precision here means enough digits so that when written out (serialized) - and read back in (deserialized)
     *         you get the exact same answer.
     * 
     *  \see  // https://stackoverflow.com/questions/22458355/what-is-the-purpose-of-max-digits10-and-how-is-it-different-from-digits10
     *        numeric_limits<T>::max_digits10
     * 
     *  \note defined here instead of header cuz needs to be after SignificantFigures::CTOR.
     */
    constexpr inline SignificantFigures SignificantFigures::kFullPrecision{SignificantFigures::FullFlag::eFull};

    /*
     ********************************************************************************
     ***************************** FloatToStringOptions *****************************
     ********************************************************************************
     */
    inline FloatToStringOptions::FloatToStringOptions (const locale& l)
        : fUseLocale_{l}
    {
    }
    constexpr FloatToStringOptions::FloatToStringOptions (PredefinedLocale p)
        : fUseCurrentLocale_{p == PredefinedLocale::eUseCurrentLocale}
    {
    }
    constexpr FloatToStringOptions::FloatToStringOptions (ios_base::fmtflags fmtFlags)
        : fFmtFlags_{fmtFlags}
    {
    }
    constexpr FloatToStringOptions::FloatToStringOptions (SignificantFigures precision)
        : fSignificantFigures_{precision}
    {
    }
    constexpr FloatToStringOptions::FloatToStringOptions (FloatFormatType scientificNotation)
        : fFloatFormat_{scientificNotation}
    {
    }
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    constexpr FloatToStringOptions::FloatToStringOptions (TrimTrailingZerosType trimTrailingZeros)
        : fTrimTrailingZeros_{trimTrailingZeros == TrimTrailingZerosType::eTrimZeros}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");

    constexpr FloatToStringOptions::FloatToStringOptions (const FloatToStringOptions& b1, const FloatToStringOptions& b2)
        : FloatToStringOptions{b1}
    {
        Memory::CopyToIf (&fSignificantFigures_, b2.fSignificantFigures_);
        Memory::CopyToIf (&fFmtFlags_, b2.fFmtFlags_);
        fUseCurrentLocale_ = b2.fUseCurrentLocale_;
        Memory::CopyToIf (&fUseLocale_, b2.fUseLocale_);
        Memory::CopyToIf (&fTrimTrailingZeros_, b2.fTrimTrailingZeros_);
        Memory::CopyToIf (&fFloatFormat_, b2.fFloatFormat_);
    }
    template <typename... ARGS>
    constexpr FloatToStringOptions::FloatToStringOptions (const FloatToStringOptions& b1, const FloatToStringOptions& b2, ARGS&&... args)
        : FloatToStringOptions{FloatToStringOptions{b1, b2}, forward<ARGS> (args)...}
    {
    }
    constexpr optional<SignificantFigures> FloatToStringOptions::GetSignificantFigures () const
    {
        return fSignificantFigures_;
    }
    constexpr optional<bool> FloatToStringOptions::GetTrimTrailingZeros () const
    {
        return fTrimTrailingZeros_;
    }
    inline locale FloatToStringOptions::GetUseLocale () const
    {
        if (fUseCurrentLocale_) {
            return locale{};
        }
        if (fUseLocale_) {
            return *fUseLocale_;
        }
        static const locale kCLocale_ = locale::classic ();
        return kCLocale_;
    }
    inline bool FloatToStringOptions::GetUsingLocaleClassic () const
    {
        if (not fUseLocale_ and not fUseCurrentLocale_) {
            return true; // very common case
        }
        return GetUseLocale () == locale::classic ();
    }
    constexpr optional<FloatFormatType> FloatToStringOptions::GetFloatFormat () const
    {
        return fFloatFormat_;
    }
    constexpr optional<ios_base::fmtflags> FloatToStringOptions::GetIOSFmtFlags () const
    {
        return fFmtFlags_;
    }

}
