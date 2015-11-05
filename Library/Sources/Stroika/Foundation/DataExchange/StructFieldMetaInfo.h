/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_
#define _Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_    1

#include    "../StroikaPreComp.h"

#include    <type_traits>
#include    <typeindex>




/**
 *
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo   This is NOT safe (annoying offsetof rules). MAYBE possible to do with ptr-to-member, but I dont think
 *              so.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /**
             *
             */
            struct  StructFieldMetaInfo {
                size_t                  fOffset;
                type_index              fTypeInfo;

                StructFieldMetaInfo (size_t fieldOffset, type_index typeInfo);
            };


            /**
             *  Greatly regret adding a macro, but it just seems SO HELPFUL (makes things much more terse).
             *  No need to use - but some may find it helpfull...
             *
             *  I don't know of any way in C++ without macro - to capture a member name (for use in decltype
             *  thing and offsetof()).
             *
             *  \note
             *      This macro uses offsetof(). According to the C++11 spec, offsetof() is only supported
             *      for 'standard-layout' objects (e.g. those without virtual functions, or other fancy
             *      c++ stuff). As near as I can tell, this always works, but we may need to revisit
             *      the approach/question (could  we use pointer to member?).
             *
             */
#define     ObjectVariantMapper_StructFieldMetaInfo(CLASS,MEMBER)\
    Stroika::Foundation::DataExchange::StructFieldMetaInfo { offsetof (CLASS, MEMBER), typeid (decltype (CLASS::MEMBER)) }


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StructFieldMetaInfo.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_*/
