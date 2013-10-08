/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_ReadOnlyRep_h_
#define _Stroika_Foundation_Characters_String_Concrete_Private_ReadOnlyRep_h_    1

#include    "../../../StroikaPreComp.h"

#include    "../../../Containers/Common.h"
#include    "../../../Debug/Assertions.h"
#include    "../../../Execution/Exceptions.h"

#include    "../../String.h"


/**
 *  \file
 *
 *
 * TODO:
 *
 */

////// DOPCUMENT SEMI_PRIVATE


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {
                namespace   Private {

                    /**
                     *This is a utility class to implement most of the basic String::_IRep functionality
                     *
                     *  explain queer wrapper class cuz protected
                     */
                    struct  ReadOnlyRep : String {
                        struct  _Rep : public String::_IRep {
                        public:
                            _Rep (const wchar_t* start, const wchar_t* end)
                                : _fStart (start)
                                , _fEnd (end) {
                            }
                            nonvirtual  void    _SetData (const wchar_t* start, const wchar_t* end) {
                                Require (_fStart <= _fEnd);
                                _fStart = start;
                                _fEnd = end;
                            }
                            virtual     size_t  GetLength () const override {
                                Assert (_fStart <= _fEnd);
                                return _fEnd - _fStart;
                            }
                            virtual     Character   GetAt (size_t index) const override {
                                Assert (_fStart <= _fEnd);
                                Require (index < GetLength ());
                                return _fStart[index];
                            }
                            virtual     const Character*    Peek () const override {
                                Assert (_fStart <= _fEnd);
                                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                                return ((const Character*)_fStart);
                            }
                            virtual pair<const Character*, const Character*> GetData () const override {
                                Assert (_fStart <= _fEnd);
                                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                                return pair<const Character*, const Character*> ((const Character*)_fStart, (const Character*)_fEnd);
                            }
                            nonvirtual  int Compare_CS_ (const Character* rhsStart, const Character* rhsEnd) const {
// TODO: Need a more efficient implementation - but this should do for starters...
                                Assert (_fStart <= _fEnd);
                                size_t lLen = GetLength ();
                                size_t rLen = (rhsEnd - rhsStart);
                                size_t length   =   min (lLen, rLen);
                                for (size_t i = 0; i < length; i++) {
                                    if (_fStart[i] != rhsStart[i]) {
                                        return (_fStart[i] - rhsStart[i].GetCharacterCode ());
                                    }
                                }
                                return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
                            }
                            nonvirtual  int Compare_CI_ (const Character* rhsStart, const Character* rhsEnd) const {
// TODO: Need a more efficient implementation - but this should do for starters...
                                Assert (_fStart <= _fEnd);
                                // Not sure wcscasecmp even helps because of convert to c-str
                                //return ::wcscasecmp (l.c_str (), r.c_str ());;
                                size_t lLen = GetLength ();
                                size_t rLen = (rhsEnd - rhsStart);
                                size_t length   =   min (lLen, rLen);
                                for (size_t i = 0; i < length; i++) {
                                    Character   lc  =   Character (_fStart[i]).ToLowerCase ();
                                    Character   rc  =   rhsStart[i].ToLowerCase ();
                                    if (lc.GetCharacterCode () != rc.GetCharacterCode ()) {
                                        return (lc.GetCharacterCode () - rc.GetCharacterCode ());
                                    }
                                }
                                return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
                            }
                            virtual int Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co) const override {
                                Require (co == CompareOptions::eWithCase or co == CompareOptions::eCaseInsensitive);
                                Assert (_fStart <= _fEnd);
                                switch (co) {
                                    case    CompareOptions::eWithCase:
                                        return Compare_CS_ (rhsStart, rhsEnd);
                                    case    CompareOptions::eCaseInsensitive:
                                        return Compare_CI_ (rhsStart, rhsEnd);
                                    default:
                                        AssertNotReached ();
                                        return 0;
                                }
                            }
                            virtual void    InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override {
                                Execution::DoThrow (UnsupportedFeatureException ());
                            }
                            virtual void    RemoveAll () override {
                                Execution::DoThrow (UnsupportedFeatureException ());
                            }
                            virtual void    SetAt (Character item, size_t index) {
                                Execution::DoThrow (UnsupportedFeatureException ());
                            }
                            virtual void    RemoveAt (size_t index, size_t amountToRemove) {
                                Execution::DoThrow (UnsupportedFeatureException ());
                            }
                            virtual void    SetLength (size_t newLength) override {
                                Execution::DoThrow (UnsupportedFeatureException ());
                            }
                            virtual const wchar_t*  c_str_peek () const  noexcept override {
                                return nullptr;
                            }
                            virtual const wchar_t*      c_str_change () override {
                                Execution::DoThrow (UnsupportedFeatureException ());
                            }

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
