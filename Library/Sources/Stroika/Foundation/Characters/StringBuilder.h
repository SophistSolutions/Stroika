/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringBuilder_h_
#define _Stroika_Foundation_Characters_StringBuilder_h_    1

#include    "../StroikaPreComp.h"

#include    "../Memory/SmallStackBuffer.h"

#include    "String.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *
             *  SUPER ROUGH DRAFT
             *
             *  NOTE - THIS IS NOT THREADSAFE!!!!!
             *
             *  @todo ADD CODE TO CHECK IN DEBUG MODE FOR DIFF THREAD ACCESS AND ASSERT.
             *
             *
             *  @see String
             *  @see .Net StringBuilder - http://msdn.microsoft.com/en-us/library/system.text.stringbuilder(v=vs.110).aspx
             *  @see Java StringBuilder - http://docs.oracle.com/javase/7/docs/api/java/lang/StringBuilder.html
             */
            class   StringBuilder {
            public:
                StringBuilder ();
                explicit StringBuilder (const String& initialValue);

            public:
                /**
                 *  add overloads
                 */
                nonvirtual  StringBuilder&  operator+= (const wchar_t* s);
                nonvirtual  StringBuilder&  operator+= (const String& s);

            public:
                /**
                 */
                nonvirtual  operator String () const;

            public:
                /**
                 *  mimic wstringstteam method
                 */
                nonvirtual  String str () const;

            private:
                Memory::SmallStackBuffer<wchar_t>   fData_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StringBuilder.inl"

#endif  /*_Stroika_Foundation_Characters_StringBuilder_h_*/
