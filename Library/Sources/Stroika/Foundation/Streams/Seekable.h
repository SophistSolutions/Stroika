/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Seekable_h_
#define _Stroika_Foundation_Streams_Seekable_h_ 1

#include    "../StroikaPreComp.h"

#include    <cstdint>

#include    "../Configuration/Common.h"



/**
 *  \file
 *
 *
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  When seeking, you can see an offset from the start (typical/start) or from the end of the
             *  stream, or from the current position)
             */
            enum class Whence : uint8_t {
                eFromStart,
                eFromCurrent,
                eFromEnd,
            };


            /**
             *  SeekOffsetType is unsigned, normally, because for most purposes its zero based.
             *  @see SignedSeekOffsetType.
             */
            typedef uint64_t SeekOffsetType;


            /**
             *  SignedSeekOffsetType is signed variant of SeekOffsetType - used to specify a seek
             *  offset which can sometimes negative (backwards).
             *
             *  @see SignedSeekOffsetType.
             */
            typedef int64_t SignedSeekOffsetType;


            /**
             * Design Overview:
             *
             *      o   Designed to be a base for TextStream and BinaryStream.
             *
             *      o   Two basic parts - a 'smart-ish' pointer wrapper, and a virtual Rep.
             *          The virtual Rep provides the API which implementers override. The
             *          public part of the Seekable class itself provides the helpers
             *          inherirted by the various Stream subclasses (e.g. BinaryInputStream
             *          and TextOutputStream).
             *
             *      o   Seek called on an input stream that has already returned EOF, may cause
             *          subsequent Read() calls to NOT be at EOF.
             *
             *      o   Offsets are in whatever unit makes sense for the kind of stream this is mixed into,
             *          so for TextInputStreams, offsets are in CHARACTERS and in Binary streams, offsets
             *          are in Bytes. This implies one CANNOT reasonably mix together Binary streams and Text
             *          streams (one combines them by use of a special TextStream that refers to another
             *          BinaryStream for actual IO).
             *
             *      o   Considered doing something like .Net CanSeek (), CanRead(), etc, but I decided it
             *          was simpler and more elegant to just keep things separate and use mixin classes
             *          like 'interfaces' - and use dynamic_cast<> to see what functionality is available
             *          on a stream. At least thats so for the implementations. The wrapper class - Seekable -
             *          does have an IsSeekable() method which just tests if we have a Seekable interface.
             *
             *      o   COULD have allowed for GetOffset() to work separately from Seek () - but decided
             *          this was a simpler API, and I could think of no cases where it was particularly
             *          useful to track a 'seek offset' but not be able to Seek() - and if it is useful -
             *          its easy enuf to do another way (e.g. wrap the stream you are using, and track
             *          read calls to it and increment your own offset).
             */
            class   Seekable {
            protected:
                class   _IRep;

            protected:
                explicit Seekable (_IRep* rep);

            public:
                /**
                 * \brief   Returns true iff this object was constructed with a seekable input stream rep.
                 *
                 *  Returns true iff this object was constructed with a seekable input stream rep. Note -
                 *  seekability cannot change over the lifetime of an object.
                 */
                nonvirtual  bool    IsSeekable () const;

            protected:
                /**
                 *  Used only be subclasses when they clear out their smart pointer.
                 */
                nonvirtual  void    _Clear ();

            public:
                /**
                 * GetOffset () returns the currently seeked offset. This is the same as Seek (eFromCurrent, 0).
                 */
                nonvirtual  SeekOffsetType  GetOffset () const;

            public:
                /**
                 * The new position, measured in bytes, is obtained by adding offset bytes to the position specified by whence.
                 * Seek () returns the new resulting position.
                 */
                nonvirtual  SeekOffsetType  Seek (SignedSeekOffsetType offset) const;
                nonvirtual  SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) const;

            private:
                _IRep*   fSeekableRep_;
            };


            /**
             */
            class   Seekable::_IRep {
            public:
                virtual SeekOffsetType      GetOffset () const                                     =   0;

            public:
                virtual SeekOffsetType      Seek (Whence whence, SignedSeekOffsetType offset)      =   0;
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_Seekable_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Seekable.inl"
