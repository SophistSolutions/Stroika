/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Seekable_h_
#define _Stroika_Foundation_Streams_Seekable_h_ 1

#include    "../StroikaPreComp.h"

#include    <cstdint>

#include    "../Configuration/Common.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  When seeking, you can see an offset from the start (typical/start) or from the end of the stream,
             *  or from the current position)
             */
            enum    Whence {
                FromStart_W,
                FromCurrent_W,
                FromEnd_W,
            };


            /**
             * SeekOffsetType is unsigned, normally, because for most purposes its zero based. See SignedSeekOffsetType.
             */
            typedef uint64_t SeekOffsetType;


            /**
             * Offset is signed, because you can use a negative # from the end or from the current position.
             */
            typedef int64_t SignedSeekOffsetType;


            /**
             * Design Overview:
             *
             *      o   Designed to be mixed with BinaryInputStream, BinaryOutputStream, TextInputStream,
             *          or TextOutputStream.
             *
             *      o   It should be mixed in as a virtual base, so an IO stream shares a COMMON notion
             *          of current offset. Note - this choice differs from iostreams, but mirrors
             *          the choice in UNIX fileio ('read and write pointers the same).
             *
             *      o   If called on an input stream that has already returned EOF, may cause
             *          subsequence Read() calls to NOT be at EOF.
             *
             *      o   Offsets are in whatever unit makes sense for the kind of stream this is mixed into,
             *          so for TextInputStreams, offsets are in CHARACTERS and in Binary streams, offsets
             *          are in Bytes. This implies one CANNOT reasonably mix together Binary streams and Text
             *          streams (one combines them by use of a special TextStream that refers to another
             *          BinaryStream for actual IO).
             *
             *      o   BinaryInputStream and BinaryOutputStream CAN be naturally mixed togehter to make
             *          an input/output stream. Similarly, they can both be mixed together with Seekable.
             *          But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
             *
             *      o   Considered doing something like .Net CanSeek (), CanRead(), etc, but I decided it
             *          was simpler and more elegant to just keep things separate and use mixin classes
             *          like 'interfaces' - and use dynamic_cast<> to see what functionality is available
             *          on a stream.
             *
             *      o   COULD have allowed for GetOffset() to work separately from Seek () - but decided
             *          this was a simpler API, and I could think of no cases where it was particularly
             *          useful to track a 'seek offset' but not be able to Seek() - and if it is useful -
             *          its easy enuf to do another way (e.g. wrap the stream you are using, and track
             *          read calls to it and increment your own offset).
             */
            class   Seekable {
            public:
                virtual ~Seekable ();
            public:
                nonvirtual  SeekOffsetType  GetOffset () const;
            protected:
                virtual SeekOffsetType  _GetOffset () const                                     =   0;

            public:
                /**
                 * The new position, measured in bytes, is obtained by adding offset bytes to the position specified by whence
                 */
                nonvirtual  SeekOffsetType  Seek (SignedSeekOffsetType offset);
                nonvirtual  SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset);
            protected:
                virtual SeekOffsetType      _Seek (Whence whence, SignedSeekOffsetType offset)      =   0;
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
