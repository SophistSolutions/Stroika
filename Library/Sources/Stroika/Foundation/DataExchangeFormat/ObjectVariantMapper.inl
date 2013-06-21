/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_inl_
#define _Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            template    <typename CLASS>
            void    ObjectVariantMapper::RegisterClass (Sequence<TYPEINFO> typeInfo)
            {
                RegisterSerializer (mkSerializerForStruct (typeid (CLASS), typeInfo));
            }

            template    <typename CLASS>
            void    ObjectVariantMapper::Deserialize (const Memory::VariantValue& v, CLASS* into)
            {
                Deserialize  (typeid (CLASS), v, reinterpret_cast<Byte*> (into));
            }
            template    <typename CLASS>
            VariantValue    ObjectVariantMapper::Serialize (const CLASS& from)
            {
                return Serialize  (typeid (CLASS), reinterpret_cast<const Byte*> (&from));
            }

        }
    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_ObjectVariantMapper_inl_*/
