/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_ReadOnly_h_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_ReadOnly_h_    1

#include    "../../StroikaPreComp.h"

#include    "../String.h"


/**
 *  \file
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {


                /*
                 *  String_ExternalMemoryOwnership_StackLifetime_ReadOnly is a subtype of String you can use to construct a String object, so long as the memory pointed to
                 * in the argument has a
                 *      o   Greater lifetime than the String_ExternalMemoryOwnership_StackLifetime_ReadOnly envelope class
                 *      o   and buffer data never changes value externally to this String represenation
                 *
                ///REVIEW - PRETTY SURE THIS IS WRONG!!!! - UNSAFE - READONLY SHOULD mean pointer passed in is CONST - so memory may NOT be modified in this case -- LGP 2012-03-28
                /// DOBLE CHECK NO ASSIMPTIONS BELOW - WRONG - LINE NOT ASSUMED ANYWHERE
                 *  Note that the memory passed in must be READ/WRITE - and may be modified by the String_ExternalMemoryOwnership_StackLifetime_ReadOnly ()!
                 *
                 *  Strings constructed with this String_ExternalMemoryOwnership_StackLifetime_ReadOnly maybe treated like normal strings - passed anywhere, and even modified via the
                 *  String APIs. However, the underlying implemenation may cache the argument const wchar_t* cString for as long as the lifetime of the envelope class,
                 *  and re-use it as needed during this time, so only call this String constructor with great care, and then - only as a performance optimization.
                 *
                 *  This particular form of String wrapper CAN be a great performance optimization when a C-string buffer is presented and one must
                 *  call a 'String' based API. The argument C-string will be used to handle all the Stroika-String operations, and never modified, and the
                 *  association will be broken when the String_ExternalMemoryOwnership_StackLifetime_ReadOnly goes out of scope.
                 *
                 *  This means its EVEN safe to use in cases where the String object might get assigned to long-lived String variables (the internal data will be
                 *  copied in that case).
                 *
                 *  For example
                 *
                 *      extern String saved;
                 *      inline  String  F(String x)         { saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
                 *
                 *
                 *      void f (const wchar_t* cs)
                 *          {
                 *              F(L"FRED";);
                 *              F(String (L"FRED"));
                 *              F(String_ExternalMemoryOwnership_StackLifetime_ReadOnly (cs));
                 *          }
                 *
                 *  These ALL do essentially the same thing, and are all equally safe. The third call to F () with String_ExternalMemoryOwnership_StackLifetime_ReadOnly()
                 *  based memory maybe more efficient than the previous two, because the string pointed to be 'cs' never needs to be copied (now malloc/copy needed).
                 *
                 *      <<TODO: not sure we have all the CTOR/op= stuff done correctly for this class - must rethink - but only needed to rethink when we do
                 *          real optimized implemenation >>
                 */
                class   String_ExternalMemoryOwnership_StackLifetime_ReadOnly : public String {
                public:
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadOnly (const wchar_t* cString);
// DOCUMENT THESE NEW EXTRA CTORS!!! NYI
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadOnly (const wchar_t* start, const wchar_t* end);
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
#include    "String_ExternalMemoryOwnership_StackLifetime_ReadOnly.inl"

#endif  /*_Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_ReadOnly_h_*/
