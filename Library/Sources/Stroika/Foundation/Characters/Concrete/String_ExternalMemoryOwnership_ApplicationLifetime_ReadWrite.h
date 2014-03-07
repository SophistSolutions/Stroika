/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite_h_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite_h_    1

#include    "../../StroikaPreComp.h"

#include    "../String.h"



/**
 *  \file
 *
 *
 * TODO:
 *
 *      @todo   Unclear how terribly useful this is. Only can really re-use the RAM while refcount is one. Elsewise we
 *              get cloning (SharedByValue). Still - this could possibly be a useful class. Consider losing
 *              however... Or relegating to POST v2?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {


                /**
                 *      String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite is a subtype of string you can
                 * use to construct a String object, so long as the memory pointed to in the argument has a
                 *      o   FULL APPLICATION LIFETIME,
                 *      o   and never changes value through that pointer
                 *
                 *      Note that the memory passed in MUST BE MODIFIABLE (READ/WRITE) - as
                 * String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite MAY modify the data in the
                 * pointer during the objects lifetime.
                 *
                 *      Strings constructed with this String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite
                 * maybe treated like normal strings - passed anywhere, and even modified via the String APIs.
                 * However, the underlying implemenation may cache the argument const wchar_t* cString
                 * indefinitely, and re-use it as needed, so only call this String constructor with a block
                 * of read-only, never changing memory, and then - only as a performance optimization.
                 *
                 *  For example
                 *      String  tmp1    =   L"FRED";
                 *      String  tmp2    =   String (L"FRED");
                 *      static  wchar_t buf[1024] = { L"FRED" };
                 *      String  tmp3    =   String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (buf);
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
                class   _DeprecatedClass_ (String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite, "Deprecated in v2.0a21 - use string directly") : public String {
                private:
                    using   inherited   =   String;

                public:
                    explicit String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (wchar_t* cString);
                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s);

                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s);

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   MyRep_;
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite.inl"

#endif  /*_Stroika_Foundation_Characters_String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite_h_*/
