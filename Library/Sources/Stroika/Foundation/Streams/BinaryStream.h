/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
                typedef shared_ptr<_IRep>   _SharedIRep;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable::_IRep - and if so - this automatically uses it.
                 */
                explicit BinaryStream (const _SharedIRep& rep);

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
                NO_COPY_CONSTRUCTOR(_IRep);
                NO_ASSIGNMENT_OPERATOR(_IRep);

            public:
                virtual ~_IRep ();
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryStream.inl"
