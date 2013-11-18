/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_inl_
#define _Stroika_Foundation_Characters_Format_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Containers/Common.h"
#include    "../Memory/SmallStackBuffer.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            namespace Private_ {
                unsigned long long int  String2UInt_ (const String& s);
                long long int  String2Int_ (const String& s);
                DISABLE_COMPILER_MSC_WARNING_START(4018)
                template    <typename T>
                T     String2IntOrUInt_ (const String& s)
                {
                    using   std::numeric_limits;
                    if (numeric_limits<T>::is_signed) {
                        long long int    l   =   String2Int_ (s);
                        if (l <= numeric_limits<T>::min ()) {
                            return numeric_limits<T>::min ();
                        }
                        if (l >= numeric_limits<T>::max ()) {
                            return numeric_limits<T>::max ();
                        }
                        return static_cast<T> (l);
                    }
                    else {
                        unsigned long long int    l   =   String2UInt_ (s);
                        if (l >= numeric_limits<T>::max ()) {
                            return numeric_limits<T>::max ();
                        }
                        return static_cast<T> (l);
                    }
                }
            }
            DISABLE_COMPILER_MSC_WARNING_END(4018)


            /*
            ********************************************************************************
            *********************************** String2Int *********************************
            ********************************************************************************
            */
            template    <typename T>
            inline  T     String2Int (const String& s)
            {
                return Private_::String2IntOrUInt_<T> (s);
            }


            /*
            ********************************************************************************
            ****************************** Float2StringOptions *****************************
            ********************************************************************************
            */
            inline  Float2StringOptions::Precision::Precision (unsigned int p) :
                fPrecision (p)
            {
            }


            /*
            ********************************************************************************
            ****************************** Float2StringOptions *****************************
            ********************************************************************************
            */
            inline  Float2StringOptions::Float2StringOptions ()
                : fPrecision ()
                , fFmtFlags ()
                , fUseLocale ()
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (UseCLocale)
                : fPrecision ()
                , fFmtFlags ()
                , fUseLocale ()
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (UseCurrentLocale)
                : fPrecision ()
                , fFmtFlags ()
                , fUseLocale (locale ())
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (const std::locale& l)
                : fPrecision ()
                , fFmtFlags ()
                , fUseLocale (l)
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (std::ios_base::fmtflags fmtFlags)
                : fPrecision ()
                , fFmtFlags (fmtFlags)
                , fUseLocale ()
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (Precision precision)
                : fPrecision (precision.fPrecision)
                , fFmtFlags ()
                , fUseLocale ()
                , fTrimTrailingZeros (true)
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Characters_Format_inl_*/
