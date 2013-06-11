/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_BufferedArray_h_
#define _Stroika_Foundation_Characters_String_BufferedArray_h_    1

#include    "../../StroikaPreComp.h"

#include    "../String.h"


/**
 *  \file
 *
 *
 * TODO:
 *      @todo   Add move CTOR
 *
 *      @todo   EITHER embed data as buffer in BufferdString - so small strings fit without malloc,
 *              or use separate buffer. Good reasons for both ways. Not sure whats best.
 *
 *              o   At least one StringRep variant (maybe the stanrdard/common string-buffer rep
 *                  which has a fixed-size buffer, and uses that INLINE, and allocates POINTER if that isn't big enuf?
 *
 *              o   PROBABLY best to just DO direct blockallocated() calls for data < fixed size
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace Concrete {


                /**
                 *  String_BufferedArray is a kind of string which maintains extra buffer space, and
                 *  is more efficient if you are going to resize your string.
                 */
                class   String_BufferedArray  : public String {
                public:
                    /*
                     * Note for the meaning of the reserve parameter, see String_BufferedArray::reserve()
                     */
                    String_BufferedArray ();
                    String_BufferedArray (size_t reserve);
                    explicit String_BufferedArray (const wchar_t* cString);
                    explicit String_BufferedArray (const wchar_t* cString, size_t reserve);
                    explicit String_BufferedArray (const wstring& str);
                    explicit String_BufferedArray (const wstring& str, size_t reserve);
                    explicit String_BufferedArray (const String& from);
                    explicit String_BufferedArray (const String& from, size_t reserve);
                    String_BufferedArray (const String_BufferedArray& s);

                public:
                    String_BufferedArray& operator= (const String_BufferedArray& s);

                public:
                    // This returns the number of characters of space available in the buffer
                    // (without doing memory allocations)
                    nonvirtual  size_t  capacity () const;

                public:
                    // Reserve the given number of characters of space. N can be any size, and is only a hint.
                    // A value of n < GetLength () will be ignored.
                    nonvirtual  void    reserve (size_t n);
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
#include    "String_BufferedArray.inl"

#endif  /*_Stroika_Foundation_Characters_String_BufferedArray_h_*/
