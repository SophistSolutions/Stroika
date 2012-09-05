/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  BinaryStream is an abstract class defining the interface to a binary source/sink of data.
             *
             *  BinaryStream is probably nearly useless (from a users point of view), in and of itself,
             *  but it helps to tie together and facilitate mixing BinaryInputStream and BinaryOutputStream
             *  classes together (given reference counting).
             *
             *  Note that BinaryStream is logically a 'smart pointer' - to an actual stream. This is then true
             *  of all its subclasses (e.g. BinaryInputStream, etc). This is very important to understand when
             *  assigning/copying BinaryStream (and subclass) objects.
             *
             *  @todo   explain better why... ease of memory management, while still having virutla hierarchy for
             *          subclassing behavior...
             *
             *  @todo   Maybe use the word null(), or nullp(), or IsNull() instead of empty().
             *
             */
            class   BinaryStream {
            protected:
                class   _IRep;

            protected:
                typedef shared_ptr<_IRep>   _SharedIRep;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                BinaryStream (const _SharedIRep& rep);

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *  empty() doesn't check the data in the stream, but instead checks if the BinaryStream smart pointer
                 *  references any actual stream.
                 *
                 *  @see clear()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  clear () doesn't clear the data in the stream, but unreferences the BinaryStream smart pointer,
                 *  so that.
                 *
                 *  @see empty()
                 */
                nonvirtual  void    clear ();


            public:
                /**
                 * \brief   Returns true iff this object was constructed with a seekable input stream rep.
                 *
                 *  Returns true iff this object was constructed with a seekable input stream rep.
                 */
                nonvirtual  bool    IsSeekable () const;

            public:
                /**
                 * \pre not empty();
                 * \pre IsSeekable();
                 */
                nonvirtual  SeekOffsetType  GetOffset () const;

            public:
                /**
                 * \pre not empty();
                 * \pre IsSeekable();
                 *
                 *  The new position, measured in bytes, is obtained by adding offset bytes to the
                 *  position specified by whence.
                 */
                nonvirtual  void        Seek (SeekOffsetType offset);
                nonvirtual  void        Seek (Whence whence, SeekOffsetType offset);

            private:
                _SharedIRep fRep_;
                Seekable*   fSeekable_;
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
