/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Manager_inl_
#define _Stroika_Foundation_Execution_Resources_Manager_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Resources {


                /*
                 ********************************************************************************
                 ************************************* Manager **********************************
                 ********************************************************************************
                 */
                inline Manager::Manager (const shared_ptr<_IRep>& rep)
                    : fRep_ (rep)
                {
                }
                inline  Accessor    Manager::ReadResource (const Name& name) const
                {
                    return fRep_->ReadResource (name);
                }
                inline Accessor Manager::_mkAccessor (const Byte* start, const Byte* end)
                {
                    return Accessor (start, end);
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Resources_Manager_inl_*/
