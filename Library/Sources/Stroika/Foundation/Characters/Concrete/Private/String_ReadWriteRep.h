/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_ReadWriteRep_h_
#define _Stroika_Foundation_Characters_String_Concrete_Private_ReadWriteRep_h_    1

#include    "../../../StroikaPreComp.h"

#include    "../../../Containers/Common.h"
#include    "../../../Debug/Assertions.h"
#include    "../../../Execution/Exceptions.h"

#include    "String_ReadOnlyRep.h"


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
                     * This is a utility class to implement most of the basic String::_IRep functionality. This implements functions that change the string, but dont GROW it,
                     * since we don't know in general we can (thats left to subtypes)
                     *
                     *  explain queer wrapper class cuz protected
                     */
                    struct  ReadWriteRep : String {
                        struct  _Rep : public ReadOnlyRep::_Rep {
                            _Rep (wchar_t* start, wchar_t* end)
                                : ReadOnlyRep::_Rep (start, end) {
                            }
                            virtual void    RemoveAll () override {
                                Assert (_fStart <= _fEnd);
                                _fEnd = _fStart;
                            }
                            virtual void    SetAt (Character item, size_t index) {
                                Assert (_fStart <= _fEnd);
                                Require (index < GetLength ());
                                PeekStart ()[index] = item.As<wchar_t> ();
                            }
                            virtual void    RemoveAt (size_t index, size_t amountToRemove) {
                                Assert (_fStart <= _fEnd);
                                Require (index + amountToRemove <= GetLength ());
                                wchar_t*    lhs =   &PeekStart () [index];
                                wchar_t*    rhs =   &lhs [amountToRemove];
                                for (size_t i = (_fEnd - _fStart) - index - amountToRemove; i > 0; i--) {
                                    *lhs++ = *rhs++;
                                }
                                _fEnd -= amountToRemove;
                                Ensure (_fStart <= _fEnd);
                            }

                            //Presume fStart is really a WRITABLE pointer
                            nonvirtual  wchar_t*    PeekStart () {
                                return const_cast<wchar_t*> (_fStart);
                            }
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
#include    "String_ReadWriteRep.inl"

#endif  /*_Stroika_Foundation_Characters_String_Concrete_Private_ReadWriteRep_h_*/
