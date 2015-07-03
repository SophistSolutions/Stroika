/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputOutputStream_h_
#define _Stroika_Foundation_Streams_BinaryInputOutputStream_h_    1

#include    "../StroikaPreComp.h"

#include    "../Memory/Common.h"

#include    "InputOutputStream.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *  TODO:
 *      @todo   I DONT THINK THERE IS ANY POINT IN THIS FILE AND IT PROBABLY CAN BE LOST. MAYBE KEEP AROUND TO COMBINE
 *              BinaryInputStream and BinaryOutputStream but InputOutputStream<Byte> seems fine
 */


#pragma message ("Warning: BinaryInputOutputStream FILE DEPRECATED")



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using Memory::Byte;


            /**
             */
            class   BinaryInputOutputStream : public InputOutputStream<Byte> {
            private:
                using inherited = InputOutputStream<Byte>;

            protected:
                /**
                @todo update docs about Seekable
                 *  _SharedIRep must NOT inherit from Seekable. However, the resulting BinaryInputOutputStream is always seekable.
                 *
                 *  \pre dynamic_cast(rep.get (), Seekable*) == nullptr
                 */
                explicit BinaryInputOutputStream (const _SharedIRep& rep);

            public:
                _DeprecatedFunction_ (nonvirtual  SeekOffsetType  ReadGetOffset () const { return InputStream<Byte>::GetOffset (); }, "Instead use IsMissing() - to be removed after v2.0a97");

            public:
                _DeprecatedFunction_ (nonvirtual  SeekOffsetType  WriteGetOffset () const { return OutputStream<Byte>::GetOffset (); }, "Instead use IsMissing() - to be removed after v2.0a97");
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryInputOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BinaryInputOutputStream_h_*/
