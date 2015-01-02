/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextStream_h_
#define _Stroika_Foundation_Streams_TextStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"

#include    "Seekable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html">Alpha-Late</a>
 *
 *  @todo   explain better why... ease of memory management, while still having virutla hierarchy for
 *          subclassing behavior...
 *
 *  @todo   Maybe use the word null(), or nullp(), or IsNull() instead of empty().
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  TextStream is an abstract class defining the interface to a text source/sink of data.
             *
             *  TextStream is probably nearly useless (from a users point of view), in and of itself,
             *  but it helps to tie together and facilitate mixing TextInputStream and TextOutputStream
             *  classes together (given reference counting).
             *
             *  Note that TextStream is logically a 'smart pointer' - to an actual stream. This is then true
             *  of all its subclasses (e.g. TextInputStream, etc). This is very important to understand when
             *  assigning/copying TextStream (and subclass) objects.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            class   TextStream  : public Seekable {
            protected:
                class   _IRep;

            protected:
                using   _SharedIRep     =   shared_ptr<_IRep>;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable::_IRep - and if so - this automatically uses it.
                 */
                explicit TextStream (const _SharedIRep& rep);

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *  empty() doesn't check the data in the stream, but instead checks if the TextStream smart pointer
                 *  references any actual stream.
                 *
                 *  @see clear()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  clear () doesn't clear the data in the stream, but unreferences the TextStream smart pointer,
                 *  so that.
                 *
                 *  @see empty()
                 */
                nonvirtual  void    clear ();


            private:
                _SharedIRep fRep_;
            };


            /**
             *
             */
            class   TextStream::_IRep {
            public:
                _IRep ();
                _IRep (const _IRep&) = delete;

            public:
                virtual ~_IRep ();

            public:
                nonvirtual  _IRep& operator= (const _IRep&) = delete;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TextStream.inl"

#endif  /*_Stroika_Foundation_Streams_TextStream_h_*/
