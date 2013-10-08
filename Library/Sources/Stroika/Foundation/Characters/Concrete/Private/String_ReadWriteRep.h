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



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   Concrete {
                namespace   Private {


                    /**
                     *  This is a utility class to implement most of the basic String::_IRep functionality.
                     *  This implements functions that change the string, but dont GROW it,
                     *  since we don't know in general we can (thats left to subtypes)
                     *
                     *  explain queer wrapper class cuz protected
                     */
                    struct  ReadWriteRep : String {
                        struct  _Rep : ReadOnlyRep::_Rep {
                        private:
                            typedef ReadOnlyRep::_Rep   inherited;

                        protected:
                            NO_DEFAULT_CONSTRUCTOR(_Rep);
                            NO_COPY_CONSTRUCTOR(_Rep);
                            NO_ASSIGNMENT_OPERATOR(_Rep);

                        protected:
                            _Rep (wchar_t* start, wchar_t* end);

                        protected:
                            //Presume fStart is really a WRITABLE pointer
                            nonvirtual  wchar_t*    _PeekStart ();

                        public:
                            virtual void    RemoveAll () override;
                            virtual void    SetAt (Character item, size_t index) override;
                            virtual void    RemoveAt (size_t index, size_t amountToRemove) override;
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
