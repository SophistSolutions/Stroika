/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_ReadOnlyRep_h_
#define _Stroika_Foundation_Characters_String_Concrete_Private_ReadOnlyRep_h_    1

#include    "../../../StroikaPreComp.h"

#include    "../../String.h"



/**
 *  \file
 *
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {
                namespace   Private {


                    using   Traversal::IteratorOwnerID;


                    /**
                     *  This is a PRIVATE utility class - designed to share code among the Stroika String implementations. Please don't
                     *  use it directly - as its details may change without notice, including subtle undocumented features of use of
                     *  member variables.
                     *
                     *  This is a utility class to implement most of the basic String::_IRep functionality
                     *
                     *  explain queer wrapper class cuz protected
                     */
                    struct  ReadOnlyRep : String {
                        struct  _Rep : String::_IRep {
                        protected:
                            _Rep () = delete;
                            _Rep (const _Rep&) = delete;
                            _Rep (const wchar_t* start, const wchar_t* end):
                                _IRep (start, end)
                            {
                            }

                        public:
                            nonvirtual  const _Rep& operator= (const _Rep&) = delete;

                            // Overrides for String::_IRep
                        public:
                            virtual void                                        InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override;
                            virtual void                                        SetAt (Character item, size_t index) override;
                            virtual void                                        RemoveAt (size_t from, size_t to) override;
                            virtual const wchar_t*                              c_str_peek () const  noexcept override;
                            virtual const wchar_t*                              c_str_change () override;

                        };
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "String_ReadOnlyRep.inl"

#endif  /*_Stroika_Foundation_Characters_String_Concrete_Private_ReadOnlyRep_h_*/
