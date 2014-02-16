/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BasicBinaryOutputStream_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/BLOB.h"

#include    "BinaryOutputStream.h"
#include    "Seekable.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *      @todo   This would be a good candidate class to rewrite using new Sequence_ChunkedArray
 *              class (when I implement it) based on Led chunked arrays).
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  Simplest to use BinaryOutputStream; BasicBinaryOutputStream can be written to, and then the BLOB of data retrieved.
             *
             *  BasicBinaryOutputStream is threadsafe - meaning Write() can safely be called from
             *  multiple threads at a time freely, and the results are interleaved without corruption. There is no guarantee
             *  of ordering, but one will finish before the next starts writing (so if they are writing structured messages,
             *  those will remain in-tact).
             *
             *  BasicBinaryOutputStream is Seekable.
             *
             *  Since BasicBinaryOutputStream keeps its data all in memory, it has the limitation that attempts to seek
             *  or write more than will fit in RAM will fail (with an exception).
             *
             *  Note - this is roughly equivilent (in terms of being able to use it as an alterantive) to std::ostringstream -
             *  except that it uses the Stroika Streams library (and none of the stdC++ streams library). But as many Stroika
             *  APIs have overloads with automatic adpaters, you can often use one or the other.
             */
            class   BasicBinaryOutputStream : public BinaryOutputStream {
            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                class   IRep_;
            public:
                BasicBinaryOutputStream ();

            public:
                /**
                 *  Convert the current contents of this BasicBinaryOutputStream into one of the "T" representations.
                 *  T can be one of:
                 *      Memory::BLOB
                 *      vector<Byte>
                 *      string
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
            };


            template    <>
            Memory::BLOB    BasicBinaryOutputStream::As () const;
            template    <>
            vector<Byte>    BasicBinaryOutputStream::As () const;
            template    <>
            string          BasicBinaryOutputStream::As () const;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BasicBinaryOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BasicBinaryOutputStream_h_*/
