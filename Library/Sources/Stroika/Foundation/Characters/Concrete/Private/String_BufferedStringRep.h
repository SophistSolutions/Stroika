/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_
#define _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_    1

#include    "../../../StroikaPreComp.h"

#include    "../../String.h"



/**
 *  \file
 *
 *
 * TODO:
 *      @todo   Evaluate and document effacacy of qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings in implementaiton.
 *
 *
 */



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
                    struct  BufferedStringRep : String {
                        struct  _Rep : public _IRep {
                        private:
                            using   inherited   =   String::_IRep;

                        protected:
                            _Rep () = delete;
                            _Rep (const _Rep&) = delete;

                        public:
                            nonvirtual  const _Rep& operator= (const _Rep&) = delete;

                        public:
                            _Rep (const wchar_t* start, const wchar_t* end);
                            _Rep (const wchar_t* start, const wchar_t* end, size_t reserve);
                            ~_Rep ();

                        public:
                            virtual     void            InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override;
                            virtual     void            SetAt (Character item, size_t index) override;
                        public:
                            virtual     const wchar_t*  c_str_peek () const noexcept override;
                            virtual     const wchar_t*  c_str_change () override;

                        protected:
                            //Presume fStart is really a WRITABLE pointer
                            nonvirtual  wchar_t*    _PeekStart ();

                            // @todo - SB private next few methods - except for use in ReserveAtLeast_()...
                            // size() function defined only so we can use Containers::ReserveSpeedTweekAddN() template
                        private:
                            nonvirtual     void            SetLength_ (size_t newLength);

                        public:
                            nonvirtual  size_t  size () const;
                            nonvirtual  size_t  capacity () const;
                            nonvirtual  void    reserve (size_t newCapacity);

                        private:
                            nonvirtual  void    ReserveAtLeast_ (size_t newCapacity);

                        private:
                            size_t      fCapacity_;
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
#include    "String_BufferedStringRep.inl"

#endif  /*_Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_*/
