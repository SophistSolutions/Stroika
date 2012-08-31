/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BinaryOutputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"

#include    "BinaryStream.h"



/*
 *  \file
 *
 *  TODO:
 *      @todo   Consider/document approaches to timeouts. We COULD have a stream class where it was a PROPERTY OF THE CLASS (or alternate API)
 *              where writes timeout after a certain point.
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             * Design Overview:
             *
             *      o   BinaryInputStream and BinaryOutputStream CAN be naturally mixed togehter to make an input/output stream. Simlarly, they can both be
             *          mixed together with Seekable. But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
             */
            class   BinaryOutputStream : public BinaryStream {
            protected:
                class   _IRep;

            protected:
                typedef shared_ptr<_IRep>   _SharedIRep;

            protected:
                /**
                 * IRep_ arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                BinaryOutputStream (const _SharedIRep& rep);

            public:
                /**
                 *
                 */
                nonvirtual  _SharedIRep GetRep () const;

            public:
                // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                // Writes always succeed fully or throw.
                nonvirtual  void    Write (const Byte* start, const Byte* end);
            };



            /**
             *
             */
            class   BinaryOutputStream::_IRep : public virtual BinaryStream::_IRep {
            public:
                _IRep ();
                NO_COPY_CONSTRUCTOR(_IRep);
                NO_ASSIGNMENT_OPERATOR(_IRep);

            public:
                /*
                 * pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                 * Writes always succeed fully or throw.
                 */
                virtual void    Write (const Byte* start, const Byte* end)         =   0;
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryOutputStream.inl"
