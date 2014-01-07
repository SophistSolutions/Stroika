/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SignalHandlers_inl_
#define _Stroika_Foundation_Execution_SignalHandlers_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ******************************** SignalHandler *********************************
             ********************************************************************************
             */
            inline  SignalHandler::SignalHandler (void (*signalHandler)(SignalID), Type type)
                : fType_ (type)
                , fCall_ (shared_ptr<function<void(SignalID)>> (new function<void(SignalID)> (signalHandler)))
            {
            }
            inline                SignalHandler::SignalHandler (const function<void(SignalID)>& signalHandler, Type type)
                : fType_ (type)
                , fCall_ (shared_ptr<function<void(SignalID)>> (new function<void(SignalID)> (signalHandler)))
            {
            }
            inline  SignalHandler::Type SignalHandler::GetType () const
            {
                return fType_;
            }
            inline  void SignalHandler::operator () (SignalID i) const
            {
                Require (fCall_.get () != nullptr);
                (*fCall_) (i);
            }
            inline  bool SignalHandler::operator== (const SignalHandler& rhs) const
            {
                return fCall_.get () == rhs.fCall_.get ();
            }
            inline  bool SignalHandler::operator!= (const SignalHandler& rhs) const
            {
                return fCall_.get () != rhs.fCall_.get ();
            }
            inline  bool SignalHandler::operator< (const SignalHandler& rhs) const
            {
                // technically not quite real... - compute address of ptr...
                return fCall_.get () < rhs.fCall_.get ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SignalHandlers_inl_*/
