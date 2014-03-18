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
                            _Rep (const wchar_t* start, const wchar_t* end);

                        public:
                            nonvirtual  const _Rep& operator= (const _Rep&) = delete;

                        protected:
                            // PROTECTED INLINE UTILITY
                            nonvirtual  void    _SetData (const wchar_t* start, const wchar_t* end);
                        protected:
                            // PROTECTED INLINE UTILITY
                            nonvirtual     size_t  _GetLength () const;
                        protected:
                            // PROTECTED INLINE UTILITY
                            nonvirtual     Character   _GetAt (size_t index) const;
                        protected:
                            // PROTECTED INLINE UTILITY
                            nonvirtual     const Character*    _Peek () const;

                            // Overrides for Iterable<Character>
                        public:
                            virtual Traversal::Iterator<Character>              MakeIterator (IteratorOwnerID suggestedOwner) const override;
                            virtual size_t                                      GetLength () const override;
                            virtual bool                                        IsEmpty () const override;
                            virtual void                                        Apply (_APPLY_ARGTYPE doToElement) const override;
                            virtual Traversal::Iterator<Character>              FindFirstThat (_APPLYUNTIL_ARGTYPE, IteratorOwnerID suggestedOwner) const override;

                            // Overrides for String::_IRep
                        public:
                            virtual Character                                   GetAt (size_t index) const override;
                            virtual const Character*                            Peek () const override;
                            virtual pair<const Character*, const Character*>    GetData () const override;
                            virtual void                                        InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override;
                            virtual void                                        SetAt (Character item, size_t index) override;
                            virtual void                                        RemoveAt (size_t from, size_t to) override;
                            virtual const wchar_t*                              c_str_peek () const  noexcept override;
                            virtual const wchar_t*                              c_str_change () override;

                        protected:
                            const wchar_t*  _fStart;
                            const wchar_t*  _fEnd;
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
