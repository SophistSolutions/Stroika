/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly_h_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly_h_    1

#include    "../../StroikaPreComp.h"

#include    "../String.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   Consider if param to String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly should use
 *              constexpr? Would that add to safety?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {


                /**
                 *      String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly is a subtype of string you can
                 * use to construct a String object, so long as the memory pointed to in the argument has a
                 *      o   FULL APPLICATION LIFETIME,
                 *      o   the member referenced never changes - is READONLY.
                 *
                 *      String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly will NOT change the memory
                 * referenced in the CTOR.
                 *
                 *      Strings constructed with this String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly
                 * maybe treated like normal strings - passed anywhere, and even modified via the String APIs.
                 *
                 *  For example
                 *      String  tmp1    =   L"FRED";
                 *      String  tmp2    =   String (L"FRED");
                 *      String  tmp3    =   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (L"FRED");
                 *
                 *      extern String saved;
                 *      inline  String  F(String x)         { saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
                 *      F(tmp1);
                 *      F(tmp2);
                 *      F(tmp3);
                 *
                 *      These ALL do essentially the same thing, and are all equally safe. The 'tmp3' implementation
                 * maybe slightly more efficent, but all are equally safe.
                 *
                 *  \em WARNING - BE VERY CAREFUL - be sure arguments have application lifetime.
                 */
                class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly : public String {
                private:
                    using   inherited   =   String;

                public:
                    /**
                     *  The constructor requires an application lifetime NUL-terminated array of characters - such as one
                     *  created with L"sample".
                     *
                     *      \req SIZE >= 1
                     *      \req cString[SIZE-1] == '\0'
                     */
                    template    <size_t SIZE>
                    explicit String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const wchar_t (&cString)[SIZE]);
                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const wchar_t* start, const wchar_t* end);
                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& src) = default;

                public:
                    nonvirtual  String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s) = default;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   MyRep_;
                };


                typedef String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly _Deprecated_ (String_Constant, "Deprecated as of v2.0a20 - see Characters::String_Constant instead");


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly.inl"

#endif  /*_Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly_h_*/
