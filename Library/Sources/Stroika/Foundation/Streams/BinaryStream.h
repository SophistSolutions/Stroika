/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryStream_h_
#define _Stroika_Foundation_Streams_BinaryStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"

#include    "Seekable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
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
             *  \brief  BinaryStream is an abstract class defining the interface to a binary source/sink
             *          of data.
             *
             *  BinaryStream is probably nearly useless (from a users point of view), in and of itself,
             *  but it helps to tie together and facilitate mixing BinaryInputStream and BinaryOutputStream
             *  classes together (given reference counting).
             *
             *  Note that BinaryStream is logically a 'smart pointer' - to an actual stream. This is then true
             *  of all its subclasses (e.g. BinaryInputStream, etc). This is very important to understand when
             *  assigning/copying BinaryStream (and subclass) objects.
             *
             */
            class   BinaryStream : public Seekable {
            protected:
                class   _IRep;

            public:
                /**
                 *  defaults to null (empty ())
                 */
                BinaryStream ();
                BinaryStream (nullptr_t);

            protected:
                using       _SharedIRep     =   shared_ptr<_IRep>;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable::_IRep - and if so - this automatically uses it.
                 */
                explicit BinaryStream (const _SharedIRep& rep);
                explicit BinaryStream (const _SharedIRep& rep, Seekable::_IRep* seekable);

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *  empty() doesn't check the data in the stream, but instead checks if the BinaryStream
                 *  smart pointer references any actual stream.
                 *
                 *  @see clear()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  clear () doesn't clear the data in the stream, but unreferences the BinaryStream
                 *  smart pointer.
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
            class   BinaryStream::_IRep {
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
#include    "BinaryStream.inl"

#endif  /*_Stroika_Foundation_Streams_BinaryStream_h_*/
