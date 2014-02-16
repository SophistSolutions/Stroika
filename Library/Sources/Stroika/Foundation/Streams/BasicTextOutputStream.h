/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicTextOutputStream_h_
#define _Stroika_Foundation_Streams_BasicTextOutputStream_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Characters/String.h"
#include    "../Configuration/Common.h"

#include    "TextOutputStream.h"
#include    "Seekable.h"



/**
 *  \file
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Characters::Character;
            using   Characters::String;

            /**
             *  \brief  Simplest to use TextOutputStream; BasicTextOutputStream can be written to, and then the text (String) of data retrieved.
             *
             *  BasicTextOutputStream is threadsafe - meaning Write() can safely be called from
             *  multiple threads at a time freely, and the results are interleaved without corruption. There is no guarantee
             *  of ordering, but one will finish before the next starts writing (so if they are writing structured messages,
             *  those will remain in-tact).
             *
             *  BasicTextOutputStream is Seekable.
             *
             *  Since BasicTextOutputStream keeps its data all in memory, it has the limitation that attempts to seek
             *  or write more than will fit in RAM will fail (with an exception).
             *
             *  Note - this is roughly equivilent (in terms of being able to use it as an alterantive) to std::owstringstream -
             *  except that it uses the Stroika Streams library (and none of the stdC++ streams library). But as many Stroika
             *  APIs have overloads with automatic adpaters, you can often use one or the other.
             */
            class   BasicTextOutputStream : public TextOutputStream {
            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                class   IRep_;
            public:
                BasicTextOutputStream ();

            public:
                /**
                 *  Convert the current contents of this BasicBinaryOutputStream into one of the "T" representations.
                 *  T can be one of:
                 *      String
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
            };


            template    <>
            String    BasicTextOutputStream::As () const;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BasicTextOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BasicTextOutputStream_h_*/
