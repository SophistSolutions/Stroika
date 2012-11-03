/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Accessor_inl_
#define _Stroika_Foundation_Execution_Resources_Accessor_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Resources {

                //  class   Accessor
                inline   Accessor::Accessor (const Byte* start, const Byte* end)
                    : fDataStart_ (start)
                    , fDataEnd_ (end)
                {
                }
                inline  const Byte* Accessor::GetDataStart () const
                {
                    return fDataStart_;
                }
                inline  const Byte* Accessor::GetDataEnd () const
                {
                    return fDataEnd_;
                }
                inline  vector<Byte>    Accessor::GetData () const
                {
                    return vector<Byte> (fDataStart_, fDataEnd_);
                }

            }
        }
    }
}

#endif  /*_Stroika_Foundation_Execution_Resources_Accessor_inl_*/
