/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextOutputStream_h_
#define _Stroika_Foundation_Streams_TextOutputStream_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Configuration/Common.h"





namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            using   Characters::Character;

            /*
             * Design Overview:
             *
             *      o   TextInputStream and TextOutputStream CAN be naturally mixed togehter to make an input/output stream. Simlarly, they can both be
             *          mixed together with Seekable. But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
             */
            class   TextOutputStream {
            protected:
                TextOutputStream ();
            public:
                virtual ~TextOutputStream ();

            public:
                // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                // Writes always succeed fully or throw.
                nonvirtual  void    Write (const Character* start, const Character* end);

            protected:
                // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                // Writes always succeed fully or throw.
                virtual void    _Write (const Character* start, const Character* end)           =   0;
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TextOutputStream.inl"
