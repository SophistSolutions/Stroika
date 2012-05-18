/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_ResourceAccessor_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_ResourceAccessor_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   Windows {

                    //  class   ResourceAccessor
                    inline  bool    ResourceAccessor::GetFound () const
                    {
                        return fDataStart != nullptr;
                    }
                    inline  const Byte* ResourceAccessor::GetDataStart () const
                    {
                        return fDataStart;
                    }
                    inline  const Byte* ResourceAccessor::GetDataEnd () const
                    {
                        return fDataEnd;
                    }
                    inline  vector<Byte>    ResourceAccessor::GetData () const
                    {
                        return vector<Byte> (fDataStart, fDataEnd);
                    }
                }
            }
        }
    }
}

#endif  /*_Stroika_Foundation_Execution_Platform_Windows_ResourceAccessor_inl_*/
