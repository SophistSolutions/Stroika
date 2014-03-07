/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_ReadWrite_h_
#define _Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_ReadWrite_h_    1

#include    "../../StroikaPreComp.h"

#include    "../String.h"


/**
 *  \file
 *
 *
 * TODO:
 *      @todo   Redo implementation of String_StackLifetime - using high-performance algorithm described in the documentation.
 *
 *      @todo   Make another pass over String_ExternalMemoryOwnership_StackLifetime_ReadOnly/ReadWrite
 *              documentation, and make clearer, and document the tricky bits loosely
 *              alluded to in the appropriate place if the API is truely DOABLE.
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
                 *  String_ExternalMemoryOwnership_StackLifetime_ReadWrite is a subtype of String you can use to
                 *  construct a String object, so long as the memory pointed to min the argument has a
                 *      o   Greater lifetime than the String_ExternalMemoryOwnership_StackLifetime_ReadWrite envelope class
                 *      o   and buffer data never changes value externally to this String represenation (but maybe changed by
                 *          the String_ExternalMemoryOwnership_StackLifetime_ReadWrite implementation)
                 *
                 *  Note that the memory passed in must be READ/WRITE - and may be modified by the
                 *  String_ExternalMemoryOwnership_StackLifetime_ReadWrite ()!
                 *
                 *  Strings constructed with this String_ExternalMemoryOwnership_StackLifetime_ReadWrite maybe treated
                 *  like normal strings - passed anywhere, and even modified via the
                 *  String APIs. However, the underlying implemenation may cache the argument 'wchar_t* cString' for as long as the lifetime of the envelope class,
                 *  and re-use it as needed during this time, so only call this String constructor with great care, and then - only as a performance optimization.
                 *
                 *  This particular form of String wrapper CAN be a great performance optimization when a C-string buffer is presented and one must
                 *  call a 'String' based API. The argument C-string will be used to handle all the Stroika-String operations, and never modified, and the
                 *  association will be broken when the String_ExternalMemoryOwnership_StackLifetime_ReadWrite goes out of scope.
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
                 *      void f ()
                 *          {
                 *              char    cs[1024] = L"FRED";
                 *              F(L"FRED";);
                 *              F(String (L"FRED"));
                 *              F(String_ExternalMemoryOwnership_StackLifetime_ReadWrite (cs));
                 *          }
                 *
                 *  These ALL do essentially the same thing, and are all equally safe. The third call to F () with String_ExternalMemoryOwnership_StackLifetime_ReadWrite()
                 *  based memory maybe more efficient than the previous two, because the string pointed to be 'cs' never needs to be copied (until its changed inside F()).
                 *
                 *      <<TODO: not sure we have all the CTOR/op= stuff done correctly for this class - must rethink - but only needed to rethink when we do
                 *          real optimized implemenation >>
                 */
                class   _Deprecated_ (String_ExternalMemoryOwnership_StackLifetime_ReadWrite, "deprecated in v2.0a21") : public String {
                private:
                    using   inherited   =   String;

                public:
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* cString);
// DOCUMENT THESE NEW EXTRA CTORS!!! NYI
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* start, wchar_t* end);
//TODO: start/end defines range of string, and bufend if 'extra bytes' usable after end - enen though not orignally part of string
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* start, wchar_t* end, wchar_t* bufEnd);
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
#include    "String_ExternalMemoryOwnership_StackLifetime_ReadWrite.inl"

#endif  /*_Stroika_Foundation_Characters_String_ExternalMemoryOwnership_StackLifetime_ReadWrite_h_*/
