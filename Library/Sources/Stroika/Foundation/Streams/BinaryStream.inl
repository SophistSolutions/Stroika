/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryStream_inl_
#define _Stroika_Foundation_Streams_BinaryStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


			/*
			 ********************************************************************************
			 ******************************** BinaryStream::_IRep ***************************
			 ********************************************************************************
			 */
            inline  BinaryStream::_IRep::_IRep ()
            {
            }
            inline  BinaryStream::_IRep::~_IRep ()
            {
            }


			/*
			 ********************************************************************************
			 ************************************ BinaryStream ******************************
			 ********************************************************************************
			 */
            inline  BinaryStream::BinaryStream ()
                : Seekable (nullptr)
                , fRep_ ()
            {
            }
            inline  BinaryStream::BinaryStream (const _SharedIRep& rep)
                : Seekable (dynamic_cast<Seekable::_IRep*> (rep.get ()))
                , fRep_ (rep)
            {
            }
            inline  BinaryStream::_SharedIRep  BinaryStream::_GetRep () const
            {
                return fRep_;
            }
            inline  bool    BinaryStream::empty () const
            {
                return fRep_.get () == nullptr;
            }
            inline  void    BinaryStream::clear ()
            {
                fRep_.reset ();
                Seekable::_Clear ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryStream_inl_*/
