/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
 *      @todo   Make rep-type available so that other sibling types can easiy use it to construct/morph
 *              into this type (maybe not needed - cuz they can use String_BufferedArray(xxx).GetRep()().
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
                class   _Deprecated_ (String_BufferedArray, "Deprecated in v2.0a21 - use String directly or StringBuilder")  : public String {
                private:
                    using   inherited   =   String;

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
