/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Seekable_h_
#define _Stroika_Foundation_Streams_Seekable_h_ 1

#include    "../StroikaPreComp.h"

#include    <cstdint>

#include    "../Configuration/Common.h"
#include    "../Configuration/Enumeration.h"

#include    "Stream.h"  //tmphack - this file going a way


// soon obsolete file





namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            class   Seekable {
            protected:
                class   _IRep;

            protected:
                explicit Seekable (_IRep* rep);

            public:
                nonvirtual  bool    IsSeekable () const;

            protected:
                nonvirtual  void    _Clear ();

            public:
                nonvirtual  SeekOffsetType  GetOffset () const;

            public:
                nonvirtual  SeekOffsetType  GetOffsetToEndOfStream () const;

            public:
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



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Seekable.inl"

#endif  /*_Stroika_Foundation_Streams_Seekable_h_*/
