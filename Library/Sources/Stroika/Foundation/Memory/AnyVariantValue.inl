/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_AnyVariantValue_inl_
#define _Stroika_Foundation_Memory_AnyVariantValue_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Memory {

            struct AnyVariantValue::IRep_ {
                virtual ~IRep_ () {}
                virtual const type_info& GetType () const = 0;
            };

            template <typename T>
            struct AnyVariantValue::TIRep_ : public IRep_ {
                T fValue;
                TIRep_ (const T& from)
                    : fValue (from)
                {
                }
                virtual const type_info& GetType () const override
                {
                    return typeid (T);
                }
            };

            /*
             ********************************************************************************
             ******************************* AnyVariantValue ********************************
             ********************************************************************************
             */
            inline AnyVariantValue::AnyVariantValue (AnyVariantValue&& from)
                : fVal_ (move (from.fVal_))
            {
            }
            template <typename T>
            inline AnyVariantValue::AnyVariantValue (T val)
                : fVal_ (make_shared<TIRep_<T>> (val))
            {
            }
            inline const type_info& AnyVariantValue::GetType () const
            {
                if (fVal_.get () == nullptr) {
                    return typeid (void);
                }
                return fVal_->GetType ();
            }
            inline bool AnyVariantValue::empty () const
            {
                return fVal_.get () == nullptr;
            }
            inline void AnyVariantValue::clear ()
            {
                fVal_.reset ();
            }
            template <typename RETURNTYPE>
            inline RETURNTYPE AnyVariantValue::As () const
            {
                Require (typeid (RETURNTYPE) == GetType ());
                Require (typeid (RETURNTYPE) != typeid (void));
                AssertNotNull (fVal_.get ());
                // Could use dynamic_cast but this should be equally safe (cuz of assert above) and more
                // efficient
                const TIRep_<RETURNTYPE>* t = reinterpret_cast<TIRep_<RETURNTYPE>*> (fVal_.get ());
                return t->fValue;
            }
            template <typename RETURNTYPE>
            inline optional<RETURNTYPE> AnyVariantValue::IfAs () const
            {
                return empty () ? optional<RETURNTYPE>{} : As<RETURNTYPE> ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Memory_AnyVariantValue_inl_*/
