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
 *  TODO:
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             *  \brief  BinaryStream is an abstract class defining the interface to a binary source/sink of data.
             *
			 *	BinaryStream is probably nearly useless (from a users point of view), in and of itself, 
			 *	but it helps to tie together and facilitate mixing BinaryInputStream and BinaryOutputStream 
			 *	classes together (given reference counting).
			 *
			 *	Note:
			 *
			 *		Subclasses (e.g. BinaryInputStream and BinaryOutputStream) use direct inheritance
			 *		instead of virtual inheritance for a number of reasons. Because of the semantics of
			 *		constructors with virtual base classes, its a bit of a pain to pass in the initial
			 *		value of the shared_ptr<> rep. Also (less clear - review - it may cause issues with
			 *		dynamic cast across virtual base?).
             */
            class   BinaryStream {
			protected:
                class   _IRep;

            protected:
                typedef shared_ptr<_IRep>   _SharedIRep;

            protected:
                /**
                 * IRep_ arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                BinaryStream (const _SharedIRep& rep);


            public:
                /**
                 *
                 */
                nonvirtual  _SharedIRep GetRep () const;

            public:
                /**
                 *
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *
                 */
                nonvirtual  void    clear ();


            public:
                /**
                 * Returns true iff this object was constructed with a seekable input stream rep.
                 */
                nonvirtual  bool    IsSeekable () const;

            public:
                /**
                 * Requires (IsSeekable ());
                 */
                nonvirtual  SeekOffsetType  GetOffset () const;

            public:
                /**
                 *	Requires (IsSeekable ());
                 *
                 *	The new position, measured in bytes, is obtained by adding offset bytes to the
				 *	position specified by whence.
                 */
                nonvirtual  void        Seek (SeekOffsetType offset);
                nonvirtual  void        Seek (Whence whence, SeekOffsetType offset);

            private:
                _SharedIRep fRep_;
                bool        fIsSeekable_;
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
