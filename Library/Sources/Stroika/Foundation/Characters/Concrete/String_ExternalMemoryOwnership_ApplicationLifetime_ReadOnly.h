/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly_h_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly_h_    1

#include    "../../StroikaPreComp.h"

#include    "../String.h"


/**
 *  \file
 *
 * TODO:
 *
 *
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
                 */
                class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly : public String {
                public:
                    explicit String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const wchar_t* cString);
                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s);

                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   MyRep_;
                };


                /**
                 *      String_Constant can safely be used to initilaize constant C-strings as Stroika strings,
                 * with a minimum of cost.
                 *
                 *      &&&&
                 *      @todo - THIS NAME MAYBE UNSAFE AND MISLEADING - MAYBE BETTER TO FORCE USE OF
                 *          String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly
                 *
                 */
                typedef String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly String_Constant;


            }


            /**
            &&&& UNSURE ABOUT TYPEDEF
             */
            typedef Concrete::String_Constant String_Constant;

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
